package org.selfbus.updater;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class UtilsTest {

    @Test
    public void testStreamToLong() {
        // a full range test takes about 40s, so try to avoid it
        // for lower two bytes we only check the signed bit range of 126..129
        ///\todo implement test of offset
        byte[] testStream = new byte[]{0, 0, 0, 0};
        int start0 = 126; // min. 0, max. 255
        int end0 = 129; // min. 0, max. 255

        int start1 = 126; // min. 0, max. 255
        int end1 = 129; // min. 0, max. 255

        int start2 = 0; // min. 0, max. 255
        int end2 = 255; // min. 0, max. 255

        int start3 = 0; // min. 0, max. 255
        int end3 = 255; // min. 0, max. 255

        for (long i = start3; i <= end3; i++) {
             for (long j = start2; j <= end2; j++) {
                  for (long k = start1; k <= end1; k++) {
                      for (long l = start0; l <= end0; l++) {
                         testStream[0] = (byte) l;
                         testStream[1] = (byte) k;
                         testStream[2] = (byte) j;
                         testStream[3] = (byte) i;
                         long expectedLong = l + (k << 8) + (j << 16) + (i << 24);
                         long result;
                         result = Utils.streamToLong(testStream, 0);
                         assertEquals(expectedLong, result);
                     }
                 }
             }
        }
    }

    @Test
    public void testLongToStream() {
        // a full range test would take way to long,
        // we only check first, last and byte borders
        ///\todo implement test of offset
        long[] underTest = {0x00L, 0x01,                        // start
                            0xFEL, 0xFFL, 0x100L,               // 1. byte border
                            0xFFFEL, 0xFFFFL, 0x10000L,         // 2. byte border
                            0xFFFFFEL, 0xFFFFFFL, 0x1000000L,   // 3. byte border
                            0xFFFFFFFEL, 0xFFFFFFFFL,           // 4. byte border and last
                            };

        long expectedLong;
        byte[] testStream = new byte[]{0, 0, 0, 0};
        for (int i = 0; i < underTest.length; i++) {
            Utils.longToStream(testStream, 0, underTest[i]);
            expectedLong = Utils.streamToLong(testStream, 0);
            assertEquals(expectedLong, underTest[i]);
        }
    }

    @Test
    public void testShortenPath() {
        int thresholdToTest = 4;
        String testFileName;
        // test windows path
        testFileName = "C:\\Users\\user\\AppData\\Local\\Selfbus\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin";
        testFileName = Utils.shortenPath(testFileName, thresholdToTest + 1);
        assertEquals("C:\\...\\Selfbus\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin", testFileName);

        // test linux path
        testFileName = Utils.shortenPath("/home/username/.cache/Selfbus-Updater/0.61/image-0x7000-34624-0xe8b27ade.bin", thresholdToTest);
        assertEquals("/.../.cache/Selfbus-Updater/0.61/image-0x7000-34624-0xe8b27ade.bin", testFileName);

        // test UNC path
        testFileName = Utils.shortenPath("\\\\share\\funstuff\\running\\from\\Selfbus\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin", thresholdToTest);
        assertEquals("\\\\...\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin", testFileName);

        // test http
        testFileName = Utils.shortenPath("http://tester.com/a1/b2/c3/d4/image-0x7000-34624-0xe8b27ade.bin", thresholdToTest);
        assertEquals("http://.../b2/c3/d4/image-0x7000-34624-0xe8b27ade.bin", testFileName);

        // test ftp
        testFileName = Utils.shortenPath("ftp://tester.com/a1/b2/c3/d4/e5/image-0x7000-34624-0xe8b27ade.bin", thresholdToTest);
        assertEquals("ftp://.../c3/d4/e5/image-0x7000-34624-0xe8b27ade.bin", testFileName);

        // test nothing to shorten
        testFileName = Utils.shortenPath("C:\\Local\\Selfbus\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin", thresholdToTest);
        assertEquals("C:\\...\\Selfbus-Updater\\Cache\\0.61\\image-0x7000-34624-0xe8b27ade.bin", testFileName);
    }
}