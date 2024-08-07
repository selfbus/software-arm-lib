package org.selfbus.updater.tests.flashdiff;

import org.selfbus.updater.BinImage;
import org.selfbus.updater.ConColors;
import org.selfbus.updater.UpdaterException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXRemoteException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;

import java.util.ArrayList;
import java.util.List;
import java.util.zip.CRC32;

public class FlashDiff {
    private final static Logger logger = LoggerFactory.getLogger(FlashDiff.class.getName());
    private static final int MINIMUM_PATTERN_LENGTH = 6; // less that this is not efficient (metadata would be larger than data)
    private static final int MAX_COPY_LENGTH = 2048-1; // 2^12 = 8 bits + 6 bits (remaining in CMD byte). Needs to match flash PAGE_SIZE?
    private static final int MAX_LENGTH_SHORT = 64-1;    // 2^6 = 6 bits (remaining in CMD byte)

    public static final int CMD_RAW = 0;
    public static final int CMD_COPY = 0b10000000;
    public static final int FLAG_SHORT = 0;
    public static final int FLAG_LONG = 0b01000000;
    public static final int ADDR_FROM_ROM = 0;
    public static final int ADDR_FROM_RAM = 0b10000000;

    private int totalBytesTransferred = 0;
    
    enum SourceType {
        FORWARD_ROM,
        BACKWARD_RAM,
    }

    static class SearchResult {
        int offset;
        int length;
        SourceType sourceType = SourceType.FORWARD_ROM;

        public SearchResult(int logestCandidateSrcOffset, int logestCandidateLength) {
            this.offset = logestCandidateSrcOffset;
            this.length = logestCandidateLength;
        }

        @Override
        public String toString() {
            return "SearchResult{" +
                    "offset=" + offset +
                    ", length=" + length +
                    ", sourceType=" + sourceType +
                    '}';
        }
    }

    public SearchResult letLongestCommonBytes(byte[] ar1, byte[] ar2, int patternOffset, int oldDataMinimumAddr, int maxLength) {
        // search as long as possible ar2[beginOffset..n] bytes (pattern) common with ar1[s..t], where n and t are up to length-1 of appropriate arrays and s is unknown
        int logestCandidateSrcOffset = 0;
        int logestCandidateLength = 0;
        for (int i = oldDataMinimumAddr; i < ar1.length; i++) {
            int j = 0;
            while ((patternOffset + j < ar2.length)
                    && (i + j < ar1.length)
                    && (ar1[i + j] == ar2[patternOffset + j])
                    && j < maxLength
            ) {
                j++;
            }
            if (j > logestCandidateLength) {
                // found better (or first) solution
                logestCandidateLength = j;
                logestCandidateSrcOffset = i;
            }
        }
        if (logestCandidateLength > 0) {
            int firstDstPage = patternOffset/FlashPage.PAGE_SIZE;
            int lastDstPage = (patternOffset+logestCandidateLength-1)/FlashPage.PAGE_SIZE;
            if (lastDstPage != firstDstPage) {
                // truncate to a single destination page
                logestCandidateLength = (firstDstPage + 1) * FlashPage.PAGE_SIZE - patternOffset;
            }
            if (logestCandidateLength >= MINIMUM_PATTERN_LENGTH) {
                logger.trace("{}", ar1[logestCandidateSrcOffset] & 0xff);
                logger.trace("{}", ar1[logestCandidateSrcOffset + 1] & 0xff);
                logger.trace("{}", ar1[logestCandidateSrcOffset + 2] & 0xff);
            }
            return new SearchResult(logestCandidateSrcOffset, logestCandidateLength);
        } else {
            return new SearchResult(0, 0);
        }
    }

    private int possiblyFinishRawBuffer(List<Byte> rawBuffer, List<Byte> outputDiffStream) {
        int outSize;
        if (!rawBuffer.isEmpty()) {
            byte cmdByte = CMD_RAW;
            if (rawBuffer.size() <= MAX_LENGTH_SHORT) {
                debug("RAW BUFFER SHORT size=" + rawBuffer.size());
                cmdByte = (byte)(cmdByte | FLAG_SHORT);
                cmdByte = (byte)(cmdByte | (rawBuffer.size() & 0b111111));  // command + 6 low bits of the length
                outputDiffStream.add(cmdByte);
                outputDiffStream.addAll(rawBuffer);
                outSize = 1 + rawBuffer.size(); // 1 byte = cmd with short length included
                rawBuffer.clear();
                return outSize;
            } else {
                debug("RAW BUFFER LONG size=" + rawBuffer.size());
                cmdByte = (byte)(cmdByte | FLAG_LONG);
                cmdByte = (byte)(cmdByte | ((rawBuffer.size() >> 8) & 0b111111));  // command + 6 high bits of the length
                byte lengthLowByte = (byte)(rawBuffer.size() & 0xff);           // 8 low bits of the length
                outputDiffStream.add(cmdByte);
                outputDiffStream.add(lengthLowByte);
                outputDiffStream.addAll(rawBuffer);
                outSize = 2 + rawBuffer.size(); // 2 bytes = cmd (6bits of length) + length (8bits of length)
                rawBuffer.clear();
                return outSize;
            }
        } else {
            return 0;
        }
    }
    
    public int getTotalBytesTransferred() {
        return totalBytesTransferred;
    }

    public void generateDiff(BinImage img1Orig, BinImage img2, FlashProgrammer flashProgrammer)
            throws InterruptedException, KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, UpdaterException {
        // TODO check if old image can be truncated for smaller new image, first test was fine
    	//BinImage img1 = new BinImage(img1Orig); // make copy to keep img1Orig untouched, this fails if arraycopy at 203, when new image is larger than old
        BinImage img1 = new BinImage(img1Orig,img2.getBinData().length); // make copy to keep img1Orig untouched, ensure old bin buffer is same size as new bin file
       
        List<Byte> outputDiffStream = new ArrayList<>();
        List<Byte> rawBuffer = new ArrayList<>();
        OldWindow w = new OldWindow();
        CRC32 crc32Block = new CRC32();
        int i = 0;
        int size = 0;
        int pages = 0;
        totalBytesTransferred = 0;
        while (i < img2.getBinData().length) {
            SearchResult rBackwardRamWindow = letLongestCommonBytes(w.getOldBinData(), img2.getBinData(), i, 0, MAX_COPY_LENGTH);
            rBackwardRamWindow.sourceType = SourceType.BACKWARD_RAM;
            //SearchResult rBackwardRamWindow = letLongestCommonBytes(img1.getBinData(), img2.getBinData(), i, 0);  // in case we would have two flash banks, ie. full old image available
            int currentPage = i / FlashPage.PAGE_SIZE;
            int firstAddressInThisPage = currentPage * FlashPage.PAGE_SIZE;
            SearchResult rForwardOldFlash = letLongestCommonBytes(img1.getBinData(), img2.getBinData(), i, 0, MAX_COPY_LENGTH);
            rForwardOldFlash.sourceType = SourceType.FORWARD_ROM;
            // which result is better, from FORWARD ROM or BACKWARD RAM?
            SearchResult bestResult = (rForwardOldFlash.length > rBackwardRamWindow.length) ? rForwardOldFlash : rBackwardRamWindow;
            if (bestResult.length >= MINIMUM_PATTERN_LENGTH) {
                size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
                logger.trace("{} bestResult={}", String.format("%08x ", i), bestResult);
                i += bestResult.length;
                size += 5;
                byte cmdByte = (byte)CMD_COPY;
                if (bestResult.length <= MAX_LENGTH_SHORT) {
                    cmdByte = (byte)(cmdByte | FLAG_SHORT | (bestResult.length & 0b111111));  // command + 6 bits of the length
                    debug("@ b=%02X i=%d CMD_COPY", (cmdByte & 0xff), i);
                    outputDiffStream.add(cmdByte);
                } else {
                    cmdByte = (byte)(cmdByte | FLAG_LONG | ((bestResult.length >> 8) & 0b111111));  // command + 6 bits from high byte of the length
                    debug("@ b=%02X i=%d CMD_COPY FLAG_LONG", (cmdByte & 0xff), i);
                    byte lengthLowByte = (byte)(bestResult.length & 0xff); // 8 low bits of the length
                    outputDiffStream.add(cmdByte);
                    outputDiffStream.add(lengthLowByte);
                }
                // 3 bytes are enough to address ROM or RAM buffer, the highest bit indicates ROM or RAM source
                byte addr1 = (byte)(bestResult.offset & 0xff);  // low byte
                byte addr2 = (byte)((bestResult.offset >> 8) & 0xff);  // middle byte
                byte addr3 = (byte)((bestResult.offset >> 16) & 0xff);  // high byte
                int addrFromFlag = bestResult.sourceType == SourceType.BACKWARD_RAM ? ADDR_FROM_RAM : ADDR_FROM_ROM;
                if (bestResult.sourceType == SourceType.BACKWARD_RAM) {
                    debug(" DO COPY FROM RAM l=%d sa=%08X", bestResult.length, bestResult.offset);
                } else {
                    debug(" DO COPY FROM ROM l=%d sa=%08X", bestResult.length, bestResult.offset);
                }
                byte[] srcData = bestResult.sourceType == SourceType.BACKWARD_RAM ? w.getOldBinData() : img1.getBinData();
                for (int k = 0; k < bestResult.length; k++) {
                    if (k % 16 == 0) {
                        debug("\n  ");
                    }
                    debug("%02X ", (srcData[bestResult.offset + k] & 0xff));
                }
                addr3 = (byte)(addr3 | addrFromFlag);
                outputDiffStream.add(addr3);
                outputDiffStream.add(addr2);
                outputDiffStream.add(addr1);
                debug("\n");
            }
            else {
                logger.trace("{} RAW: {}", String.format("%08x", i), String.format("%02x", img2.getBinData()[i]));

                debug("@ b=%02X i=%d raw", (img2.getBinData()[i] & 0xff), i);
                rawBuffer.add(img2.getBinData()[i]);
                i++;
                debug("\n");
            }
            if (i%FlashPage.PAGE_SIZE == 0) {
                // passed to new page
                size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
                crc32Block.reset();
                crc32Block.update(img2.getBinData(), i - FlashPage.PAGE_SIZE, FlashPage.PAGE_SIZE);
                //p = new FlashPage(img1.getBinData(), i);

                debug("# FLASH PAGE startAddrOfPageToBeFlashed=%08X", i*FlashPage.PAGE_SIZE);
                for (int k = pages*FlashPage.PAGE_SIZE; (k < (pages+1)*FlashPage.PAGE_SIZE && k < img2.getBinData().length); k++) {
                    if (k % 16 == 0) {
                        debug("\n  ");
                    }
                    debug(String.format("%02X ", (img2.getBinData()[k] & 0xff)));
                }
                debug("\n");
              
                System.out.print("Page " + pages + ", ");
                flashProgrammer.sendCompressedPage(outputDiffStream, crc32Block.getValue());
                outputDiffStream.clear();
                pages++;
                // emulate we have loaded the original page from ROM to RAM and written new page to ROM
                w.fillNextPage(img1.getBinData(), i - FlashPage.PAGE_SIZE); // backup old data from ROM to RAM
                System.arraycopy(img2.getBinData(), i - FlashPage.PAGE_SIZE, img1.getBinData(), i - FlashPage.PAGE_SIZE, FlashPage.PAGE_SIZE); // emulatae write of new data to ROM
            }
        }
        size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
        if (!outputDiffStream.isEmpty()) {
            crc32Block.reset();
            crc32Block.update(img2.getBinData(), img2.getBinData().length - (img2.getBinData().length % FlashPage.PAGE_SIZE), img2.getBinData().length % FlashPage.PAGE_SIZE);

            debug("# FLASH PAGE startAddrOfPageToBeFlashed=%08X", i*FlashPage.PAGE_SIZE);
            debug("\n  ");
            for (int k = pages*FlashPage.PAGE_SIZE; (k < (pages+1)*FlashPage.PAGE_SIZE && k < img2.getBinData().length); k++) {
                debug("%02X ", (img2.getBinData()[k] & 0xff));
                if (k % 16 == 0) {
                    debug("\n  ");
                }
            }
            debug("\n");

            System.out.print("Page " + pages + ", ");
            flashProgrammer.sendCompressedPage(outputDiffStream, crc32Block.getValue());
            totalBytesTransferred = size;
            logger.debug("OK! Total diff stream length={}{}{} bytes", ConColors.BRIGHT_GREEN, size, ConColors.RESET);
        }
        //dumpSideBySide(img1, img2);
    }

    protected void debug(String format, Object... args) {
    	//System.out.format(format, args);
    }
}
