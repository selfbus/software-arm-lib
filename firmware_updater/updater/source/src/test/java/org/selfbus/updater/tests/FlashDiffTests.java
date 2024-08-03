package org.selfbus.updater.tests;

import org.junit.jupiter.api.Test;
import org.selfbus.updater.BinImage;
import org.selfbus.updater.UpdaterException;
import org.selfbus.updater.tests.flashdiff.Decompressor;
import org.selfbus.updater.tests.flashdiff.FlashDiff;
import org.selfbus.updater.tests.flashdiff.FlashPage;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.concurrent.atomic.AtomicInteger;

public class FlashDiffTests {
    private void performTest(BinImage img1, BinImage img2) {
        int pages1 = (int) Math.ceil(img2.getBinData().length / (double) FlashPage.PAGE_SIZE);
        int pages2 = (int) Math.ceil(img2.getBinData().length / (double)FlashPage.PAGE_SIZE);
        int pagesMax = pages1 > pages2 ? pages1 : pages2;
        BinImage rom = new BinImage(img1, pagesMax * FlashPage.PAGE_SIZE);
        BinImage img2wholePages = new BinImage(img2, pagesMax * FlashPage.PAGE_SIZE);
        FlashDiff differ = new FlashDiff();
        AtomicInteger pageNumber = new AtomicInteger();
        Decompressor decompressor = new Decompressor(rom, (oldPagesRam, page) -> {
            // backup page to be flashed to RAM
            oldPagesRam.fillNextPage(rom.getBinData(), pageNumber.get() * FlashPage.PAGE_SIZE);
            // process decompressed page
            System.out.println("Page decompressed");
            // emulate ROM page flash (rom is our ROM in the MCU)
            System.arraycopy(page.getOldBinData(), 0, rom.getBinData(), pageNumber.get() * FlashPage.PAGE_SIZE, FlashPage.PAGE_SIZE);
            FlashDiffUtils.dumpSideBySide(img2wholePages.getBinData(), rom.getBinData(), pageNumber.get() * FlashPage.PAGE_SIZE);
            pageNumber.incrementAndGet();
            //System.exit(1);
        });
        // TODO
//        differ.generateDiff(img1, img2, outputDiffStream -> {
//            // process compressed page
//            for (byte b : outputDiffStream) {
//                decompressor.putByte(b);
//            }
//            decompressor.pageCompleted();
//        });
    }

    @Test
    public void testDiff() throws URISyntaxException {
        // test of upgrade from old version to newer (longer)
        ClassLoader contextClassLoader = Thread.currentThread().getContextClassLoader();
        URI uri1 = contextClassLoader.getResource("updater.ino.slto.v1.hex").toURI();
        URI uri2 = contextClassLoader.getResource("updater.ino.slto.v2.hex").toURI();
        BinImage img1 = BinImage.readFromHex(uri1.getPath());
        BinImage img2 = BinImage.readFromHex(uri2.getPath());
        performTest(img1, img2);
    }

    @Test
    public void testDiff2() throws URISyntaxException, UpdaterException {
        // test of new firmware into empty MCU
        URI uri2 = Thread.currentThread().getContextClassLoader().getResource("updater.ino.slto.v2.hex").toURI();
        BinImage img2 = BinImage.readFromHex(uri2.getPath());
        BinImage img1 = BinImage.dummyFilled(img2.getBinData().length, 0xff);
        performTest(img1, img2);
    }
}
