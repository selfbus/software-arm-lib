package org.selfbus.updater.mode.differential;

import org.selfbus.updater.BinImage;
import org.selfbus.updater.UpdaterException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;

import java.util.ArrayList;
import java.util.List;
import java.util.zip.CRC32;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

public class FlashDiff {
    private final static Logger logger = LoggerFactory.getLogger(FlashDiff.class);
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
        final int offset;
        final int length;
        SourceType sourceType = SourceType.FORWARD_ROM;

        public SearchResult(int longestCandidateSrcOffset, int longestCandidateLength) {
            this.offset = longestCandidateSrcOffset;
            this.length = longestCandidateLength;
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

    private SearchResult letLongestCommonBytes(byte[] ar1, byte[] ar2, int patternOffset, int oldDataMinimumAddr, int maxLength) {
        // search as long as possible ar2[beginOffset...n] bytes (pattern) common with ar1[s...t], where n and t are up to length-1 of appropriate arrays and s is unknown
        int longestCandidateSrcOffset = 0;
        int longestCandidateLength = 0;
        for (int i = oldDataMinimumAddr; i < ar1.length; i++) {
            int j = 0;
            while ((patternOffset + j < ar2.length)
                    && (i + j < ar1.length)
                    && (ar1[i + j] == ar2[patternOffset + j])
                    && j < maxLength
            ) {
                j++;
            }
            if (j > longestCandidateLength) {
                // found better (or first) solution
                longestCandidateLength = j;
                longestCandidateSrcOffset = i;
            }
        }
        if (longestCandidateLength > 0) {
            int firstDstPage = patternOffset/FlashPage.PAGE_SIZE;
            int lastDstPage = (patternOffset+longestCandidateLength-1)/FlashPage.PAGE_SIZE;
            if (lastDstPage != firstDstPage) {
                // truncate to a single destination page
                longestCandidateLength = (firstDstPage + 1) * FlashPage.PAGE_SIZE - patternOffset;
            }
            if (longestCandidateLength >= MINIMUM_PATTERN_LENGTH) {
                logger.trace("{} {} {}", ar1[longestCandidateSrcOffset] & 0xff, ar1[longestCandidateSrcOffset + 1] & 0xff,
                        ar1[longestCandidateSrcOffset + 2] & 0xff);
            }
            return new SearchResult(longestCandidateSrcOffset, longestCandidateLength);
        } else {
            return new SearchResult(0, 0);
        }
    }

    private int possiblyFinishRawBuffer(List<Byte> rawBuffer, List<Byte> outputDiffStream) {
        if (rawBuffer.isEmpty()) {
            return 0;
        }

        int outSize;
        byte cmdByte;
        if (rawBuffer.size() <= MAX_LENGTH_SHORT) {
            logger.trace("RAW BUFFER SHORT size={}", rawBuffer.size());
            cmdByte = (byte)(CMD_RAW | FLAG_SHORT);
            cmdByte = (byte)(cmdByte | (rawBuffer.size() & 0b111111));  // command + 6 low bits of the length
            outputDiffStream.add(cmdByte);
            outSize = 1; // 1 byte = cmd with short length included
        }
        else {
            logger.trace("RAW BUFFER LONG size={}", rawBuffer.size());
            cmdByte = (byte)(CMD_RAW | FLAG_LONG);
            cmdByte = (byte)(cmdByte | ((rawBuffer.size() >> 8) & 0b111111));  // command + 6 high bits of the length
            byte lengthLowByte = (byte)(rawBuffer.size() & 0xff);           // 8 low bits of the length
            outputDiffStream.add(cmdByte);
            outputDiffStream.add(lengthLowByte);
            outSize = 2; // 2 bytes = cmd (6bits of length) + length (8bits of length)
        }
        outputDiffStream.addAll(rawBuffer);
        outSize += rawBuffer.size();
        rawBuffer.clear();
        return outSize;
    }
    
    public int getTotalBytesTransferred() {
        return totalBytesTransferred;
    }

    public void generateDiff(BinImage img1Orig, BinImage img2, FlashProgrammer flashProgrammer)
            throws InterruptedException, KNXTimeoutException, KNXLinkClosedException, UpdaterException {
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
            /*
            SearchResult rBackwardRamWindow = letLongestCommonBytes(img1.getBinData(), img2.getBinData(), i, 0);  // in case we would have two flash banks, i.e. full old image available
            int currentPage = i / FlashPage.PAGE_SIZE;
            int firstAddressInThisPage = currentPage * FlashPage.PAGE_SIZE;
            */
            SearchResult rForwardOldFlash = letLongestCommonBytes(img1.getBinData(), img2.getBinData(), i, 0, MAX_COPY_LENGTH);
            rForwardOldFlash.sourceType = SourceType.FORWARD_ROM;
            // which result is better, from FORWARD ROM or BACKWARD RAM?
            SearchResult bestResult = (rForwardOldFlash.length > rBackwardRamWindow.length) ? rForwardOldFlash : rBackwardRamWindow;
            if (bestResult.length >= MINIMUM_PATTERN_LENGTH) {
                size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
                logger.trace("{} bestResult={}", String.format("%08x ", i), bestResult);
                i += bestResult.length;
                size += 1;
                byte cmdByte = (byte)CMD_COPY;
                if (bestResult.length <= MAX_LENGTH_SHORT) {
                    cmdByte = (byte)(cmdByte | FLAG_SHORT | (bestResult.length & 0b111111));  // command + 6 bits of the length
                    logger.trace("@ {} CMD_COPY", String.format("b=%02X i=%d", (cmdByte & 0xff), i));
                    outputDiffStream.add(cmdByte);
                } else {
                    cmdByte = (byte)(cmdByte | FLAG_LONG | ((bestResult.length >> 8) & 0b111111));  // command + 6 bits from high byte of the length
                    logger.trace("@ {} CMD_COPY FLAG_LONG", String.format("b=%02X i=%d", (cmdByte & 0xff), i));
                    byte lengthLowByte = (byte)(bestResult.length & 0xff); // 8 low bits of the length
                    outputDiffStream.add(cmdByte);
                    size += 1;
                    outputDiffStream.add(lengthLowByte);
                }
                // 3 bytes are enough to address ROM or RAM buffer, the highest bit indicates ROM or RAM source
                byte addr1 = (byte)(bestResult.offset & 0xff);  // low byte
                byte addr2 = (byte)((bestResult.offset >> 8) & 0xff);  // middle byte
                byte addr3 = (byte)((bestResult.offset >> 16) & 0xff);  // high byte
                int addrFromFlag = bestResult.sourceType == SourceType.BACKWARD_RAM ? ADDR_FROM_RAM : ADDR_FROM_ROM;
                String location;
                if (bestResult.sourceType == SourceType.BACKWARD_RAM) {
                    location = "RAM";
                } else {
                    location = "ROM";
                }
                logger.trace("DO COPY FROM {} {}", location, String.format("l=%d sa=%08X", bestResult.length,  bestResult.offset));
                byte[] srcData = bestResult.sourceType == SourceType.BACKWARD_RAM ? w.getOldBinData() : img1.getBinData();
                StringBuilder traceMessage = new StringBuilder();
                for (int k = 0; k < bestResult.length; k++) {
                    if (k % 16 == 0) {
                        logger.trace(traceMessage.toString());
                        traceMessage = new StringBuilder();
                    }
                    traceMessage.append(String.format("%02X ", (srcData[bestResult.offset + k] & 0xff)));
                }
                logger.trace(traceMessage.toString());

                size += 3;
                addr3 = (byte)(addr3 | addrFromFlag);
                outputDiffStream.add(addr3);
                outputDiffStream.add(addr2);
                outputDiffStream.add(addr1);
            }
            else {
                logger.trace("{} raw", String.format("%08x RAW: %02x@ b=%02X i=%d",
                        i, img2.getBinData()[i], (img2.getBinData()[i] & 0xff), i));
                rawBuffer.add(img2.getBinData()[i]);
                i++;
            }
            if (i%FlashPage.PAGE_SIZE == 0) {
                // passed to new page
                size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
                crc32Block.reset();
                crc32Block.update(img2.getBinData(), i - FlashPage.PAGE_SIZE, FlashPage.PAGE_SIZE);
                //p = new FlashPage(img1.getBinData(), i);

                logger.trace("# FLASH PAGE startAddrOfPageToBeFlashed={}",
                        String.format("%08X", i * FlashPage.PAGE_SIZE));
                StringBuilder traceMessage = new StringBuilder();
                for (int k = pages*FlashPage.PAGE_SIZE; (k < (pages+1)*FlashPage.PAGE_SIZE && k < img2.getBinData().length); k++) {
                    if (k % 16 == 0) {
                        logger.trace(traceMessage.toString());
                        traceMessage = new StringBuilder();
                    }
                    traceMessage.append(String.format("%02X ", (img2.getBinData()[k] & 0xff)));
                }
                logger.trace(traceMessage.toString());
              
                logger.trace("Page {}, ",pages);
                flashProgrammer.sendCompressedPage(outputDiffStream, crc32Block.getValue());
                outputDiffStream.clear();
                pages++;
                // emulate we have loaded the original page from ROM to RAM and written new page to ROM
                w.fillNextPage(img1.getBinData(), i - FlashPage.PAGE_SIZE); // backup old data from ROM to RAM
                System.arraycopy(img2.getBinData(), i - FlashPage.PAGE_SIZE, img1.getBinData(), i - FlashPage.PAGE_SIZE, FlashPage.PAGE_SIZE); // emulate write of new data to ROM
            }
        }
        size += possiblyFinishRawBuffer(rawBuffer, outputDiffStream);
        if (!outputDiffStream.isEmpty()) {
            crc32Block.reset();
            crc32Block.update(img2.getBinData(), img2.getBinData().length - (img2.getBinData().length % FlashPage.PAGE_SIZE), img2.getBinData().length % FlashPage.PAGE_SIZE);
            logger.trace("# FLASH PAGE startAddrOfPageToBeFlashed={}", String.format("%08X", i * FlashPage.PAGE_SIZE));
            StringBuilder traceMessage = new StringBuilder();
            for (int k = pages*FlashPage.PAGE_SIZE; (k < (pages+1)*FlashPage.PAGE_SIZE && k < img2.getBinData().length); k++) {
                traceMessage.append(String.format("%02X ", (img2.getBinData()[k] & 0xff)));
                if (k % 16 == 0) {
                    logger.trace(traceMessage.toString());
                    traceMessage = new StringBuilder();
                }
            }
            logger.trace(traceMessage.toString());

            logger.trace("Page {}, ", pages);
            flashProgrammer.sendCompressedPage(outputDiffStream, crc32Block.getValue());
            totalBytesTransferred = size;
            logger.trace("OK! Total diff stream length={} bytes", ansi().fgBright(GREEN).a(size).reset().toString());
        }
        //dumpSideBySide(img1, img2);
    }
}
