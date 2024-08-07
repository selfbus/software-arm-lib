package org.selfbus.updater.tests.flashdiff;

import org.selfbus.updater.BinImage;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Arrays;

/**
 * Apply diff stream and produce one page to be flashed
 * (based on diff stream, original ROM content, and RAM buffer to store some latest ROM pages already flashed)
 */
public class Decompressor {
    private final static Logger logger = LoggerFactory.getLogger(Decompressor.class.getName());
    private enum State {
        EXPECT_COMMAND_BYTE,
        EXPECT_COMMAND_PARAMS,
        EXPECT_RAW_DATA
    }

    private final byte[] cmdBuffer = new byte[5];
    private int expectedCmdLength = 0;
    private int cmdBufferLength = 0;
    private final byte[] scratchpad = new byte[FlashPage.PAGE_SIZE];
    private int scratchpadIndex;
    private int rawLength;
    private State state = State.EXPECT_COMMAND_BYTE;
    private final DecompressorListener listener;
    private final BinImage rom;
    private final OldWindow oldPagesRam = new OldWindow();

    public Decompressor(BinImage rom, DecompressorListener listener) {
        this.rom = rom;
        this.listener = listener;
    }

    private int getLength() {
        if ((cmdBuffer[0] & FlashDiff.FLAG_LONG) == FlashDiff.FLAG_LONG) {
            return ((cmdBuffer[0] & 0b111111) << 8) | (cmdBuffer[1] & 0xff);
        } else {
            return (cmdBuffer[0] & 0b111111);
        }
    }

    private boolean isCopyFromRam() {
        if ((cmdBuffer[0] & FlashDiff.FLAG_LONG) == FlashDiff.FLAG_LONG) {
            return (cmdBuffer[2] & FlashDiff.ADDR_FROM_RAM) == FlashDiff.ADDR_FROM_RAM;
        } else {
            return (cmdBuffer[1] & FlashDiff.ADDR_FROM_RAM) == FlashDiff.ADDR_FROM_RAM;
        }
    }

    private int getCopyAddress() {
        if ((cmdBuffer[0] & FlashDiff.FLAG_LONG) == FlashDiff.FLAG_LONG) {
            return ((cmdBuffer[2] & 0b1111111) << 16) | ((cmdBuffer[3] & 0xff) << 8) | (cmdBuffer[4] & 0xff);
        } else {
            return ((cmdBuffer[1] & 0b1111111) << 16) | ((cmdBuffer[2] & 0xff) << 8) | (cmdBuffer[3] & 0xff);
        }
    }

    private void resetStateMachine() {
        state = State.EXPECT_COMMAND_BYTE;
    }

    public void pageCompleted() {
        listener.flashPage(oldPagesRam, new FlashPage(scratchpad, 0));
        scratchpadIndex = 0;
        Arrays.fill(scratchpad, (byte)0);   // only get the last (incomplete) page padded with 0 for unit tests, otherwise not relevant
    }

    public void putByte(byte data) {
        logger.trace("Decompressor processing new byte {}, state={}", (data & 0xff), state);
        switch (state) {
            case EXPECT_COMMAND_BYTE:
                cmdBuffer[0] = data;
                cmdBufferLength = 1;
                expectedCmdLength = 1;
                if ((data & FlashDiff.CMD_COPY) == FlashDiff.CMD_COPY) {
                    expectedCmdLength += 3; // 3 more bytes of source address
                }
                if ((data & FlashDiff.FLAG_LONG) == FlashDiff.FLAG_LONG) {
                    expectedCmdLength += 1; // 1 more byte for longer length
                }
                if (expectedCmdLength > 1) {
                    state = State.EXPECT_COMMAND_PARAMS;
                } else {
                    state = State.EXPECT_RAW_DATA;
                    rawLength = 0;
                }
                break;
            case EXPECT_COMMAND_PARAMS:
                cmdBuffer[cmdBufferLength++] = data;
                if (cmdBufferLength >= expectedCmdLength) {
                    // we have all params of the command
                    if ((cmdBuffer[0] & FlashDiff.CMD_COPY) == FlashDiff.CMD_COPY) {
                        // perform copy
                        if (isCopyFromRam()) {
                            logger.trace("COPY FROM RAM index={} length={} from addr={}", scratchpadIndex, getLength(), getCopyAddress());
                            System.arraycopy(oldPagesRam.getOldBinData(), getCopyAddress(), scratchpad, scratchpadIndex, getLength());
                        } else {
                            logger.trace("COPY FROM ROM index={} length={} from addr={}", scratchpadIndex, getLength(), getCopyAddress());
                            logger.trace("{}", rom.getBinData()[getCopyAddress()] & 0xff);
                            logger.trace("{}", rom.getBinData()[getCopyAddress()+1] & 0xff);
                            logger.trace("{}", rom.getBinData()[getCopyAddress()+2] & 0xff);
                            System.arraycopy(rom.getBinData(), getCopyAddress(), scratchpad, scratchpadIndex, getLength());
                        }
                        scratchpadIndex += getLength();
                        // and finish command
                        resetStateMachine();
                    } else {
                        // next, read raw data
                        state = State.EXPECT_RAW_DATA;
                        rawLength = 0;
                    }
                } // else expect more params of the command
                break;
            case EXPECT_RAW_DATA:
                // store data read to scratchpad
                scratchpad[scratchpadIndex++] = data;
                rawLength++;
                if (rawLength >= getLength()) {
                    // we have all RAW data, reset state machine
                    resetStateMachine();
                }
        }
    }


}
