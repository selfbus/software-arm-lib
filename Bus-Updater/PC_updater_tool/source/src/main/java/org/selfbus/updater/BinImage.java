package org.selfbus.updater;

import cz.jaybee.intelhex.Parser;
import cz.jaybee.intelhex.Region;
import cz.jaybee.intelhex.listeners.BinWriter;
import cz.jaybee.intelhex.listeners.RangeDetector;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.zip.CRC32;

public class BinImage {
    private final static Logger logger = LoggerFactory.getLogger(BinImage.class.getName());
    private byte[] binData;
    private final long startAddress;
    private final long endAddress;
    private long crc32;

    public BinImage(BinImage source) {
        this.binData = Arrays.copyOf(source.binData, source.binData.length);
        this.startAddress = source.startAddress;
        this.endAddress = source.endAddress;
        this.crc32 = source.crc32;
    }

    public BinImage(BinImage source, int newLength) {
        ///\todo sanity checks for newLength and addresses
        this.binData = Arrays.copyOf(source.binData, newLength);
        this.startAddress = source.startAddress;
        this.endAddress = source.endAddress;
        this.calculateCrc32();
    }

    private BinImage(byte[] binData, long startAddress)  {
        this.binData = binData;
        this.startAddress = startAddress;
        this.endAddress = startAddress + this.binData.length - 1;
        this.calculateCrc32();
    }

    private BinImage(byte[] binData, long startAddress, long endAddress) throws UpdaterException {
        if (binData.length != (endAddress - startAddress + 1)) {
            throw new UpdaterException(String.format("Mismatch in startAddress %d, endAddress %d, length %d",
                            startAddress, startAddress, binData.length));
        }
        this.binData = binData;
        this.startAddress = startAddress;
        this.endAddress = endAddress;
        this.calculateCrc32();
    }

    public static BinImage readFromHex(String filename) {
        try (FileInputStream hexFis = new FileInputStream(filename)) {
            // init parser
            Parser parser = new Parser(hexFis);

            // 1st iteration - calculate maximum output range
            RangeDetector rangeDetector = new RangeDetector();
            parser.setDataListener(rangeDetector);
            parser.parse();
            hexFis.getChannel().position(0);
            Region outputRegion = rangeDetector.getFullRangeRegion();
            long startAddress = outputRegion.getAddressStart();
            long endAddress = outputRegion.getAddressEnd();

            // 2nd iteration - actual write of the output
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            BinWriter writer = new BinWriter(outputRegion, os, false);
            parser.setDataListener(writer);
            parser.parse();
            byte[] binData = os.toByteArray();

            return new BinImage(binData, startAddress, endAddress);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public static BinImage readFromBin(String filename, long startAddress) {
        try (FileInputStream hexFis = new FileInputStream(filename)) {
            byte[] binData = Files.readAllBytes(new File(filename).toPath());
            return new BinImage(binData, startAddress);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public static BinImage copyFromArray(byte[] byteArray, long startAddress) throws UpdaterException {
        return new BinImage(Arrays.copyOf(byteArray, byteArray.length), startAddress, startAddress + byteArray.length - 1);
    }

    public static BinImage dummyFilled(int size, int fillByte) throws UpdaterException {
        byte[] binData = new byte[size];
        Arrays.fill(binData, (byte)fillByte);
        return new BinImage(binData, 0, size - 1);
    }

    public final long startAddress() {
        return this.startAddress;
    }

    public final long endAddress() {
        return this.endAddress;
    }

    public final long length() {
        return this.getBinData().length;
    }

    public final long crc32() {
        return this.crc32;
    }

    public byte[] getBinData() {
        return binData;
    }

    private void calculateCrc32() {
        CRC32 crc32File = new CRC32();
        crc32File.update(binData, 0, binData.length);
        this.crc32 = crc32File.getValue();
    }

    public final boolean writeToBinFile(String fileName) {
        File binFile = new File(fileName);
        if (!binFile.getParentFile().exists()) {
            if (!binFile.getParentFile().mkdirs()) {
                logger.warn("{}Could not create bin-file directory {}{}",
                        ConColors.RED, binFile.getParentFile().toString(), ConColors.RESET);
            }
            return false;
        }

        try (FileOutputStream fos = new FileOutputStream(binFile)) {
            fos.write(binData);
            return true;
        } catch (IOException e) {
            logger.warn("{}Could not write bin-file {}{}",
                    ConColors.RED, binFile.getPath(), ConColors.RESET);
        }
        return false;
    }

    public final String toString() {
        return String.format("start 0x%04X, end 0x%04X, length %d, crc32 0x%04X",
                startAddress, endAddress, binData.length, crc32());
    }

}
