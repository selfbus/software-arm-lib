package org.selfbus.updater.mode.differential;

public interface DecompressorListener {
    void flashPage(OldWindow oldPagesRam, FlashPage page);
}
