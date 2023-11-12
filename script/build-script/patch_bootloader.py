"""
 Copyright (c) 2022 Martin Glueck <martin@mangari.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
"""

import argparse
import configparser
import os
import struct

size2code_map = {1 : "B", 2: "H", 4: "I"}

def read_hex_file (file_name) :
    data    = {}
    addr    = 0
    for line in open (file_name) :
        line   = line.strip () ## remove the EOL
        rcount = int (line [1:3], 16)
        raddr  = int (line [3:7], 16)
        rtype  = int (line [7:9], 16)
        rdata  = bytearray.fromhex (line [9:-2])
        if rtype ==  2 :
            addr = raddr * 16
        elif rtype == 3 :
            ## save the start address for later
            data ["START"] = rdata
        elif rtype == 0 :
            for b in rdata :
                data [addr] = b
                addr       += 1
    return data
# end def read_hex_file

def save_hex_file (file_name, data) :
    hex_file     = []
    ## extract the start address (if found in the original HEX file)
    start        = data.pop ("START", (0, 0, 0, 0))
    def _add_line (rtype, addr, * data) :
        line = ":%02X%04X%02X" % (len (data), addr, rtype)
        for d in data :
            line += "%02X" % d
        cs        = \
            ( 0xFF 
            ^ ( sum (bytearray.fromhex (line [1:])) 
              & 0xFF
              )
            ) + 1
        line     += "%02X" % (cs & 0xFF, )
        hex_file.append (line)
    # end def _add_line
    addr         = None
    line_addr    = None
    line         = []
    max_per_line = 16
    for baddr, byte in data.items () :
        if addr is None :
            addr = baddr
            if addr != 0 :
                offset = baddr // 16
                if offset != 0:
                    _add_line (2, 0, offset >> 8, offset & 0xFF)
            line_addr = addr
        if addr != baddr :
            breakpoint ()
        line.append (byte)
        addr += 1
        if len (line) == max_per_line :
            _add_line (0, line_addr, * line)
            line_addr += len (line)
            line       = []
    if line :
        _add_line (0, line_addr, * line)
    _add_line (3, 0, * start)
    _add_line (1, 0)
    with open (file_name, "w") as f :
        for l in hex_file :
            print (l, file = f)
        print ("%s created" % file_name)
# end def save_hex_file

def read_variant_definition (file_name) :
    variants   = {}
    ini        = configparser.ConfigParser ()
    ini.read (file_name)
    defines    = {}
    layout     = {}
    used_bytes = {}
    for o in ini.options ("DEFINES") :
        defines [o] = ini.getint ("DEFINES", o)
    for o in ini.options ("CONFIG_SECTION") :
        parts = ini.get ("CONFIG_SECTION", o).split (",")
        addr  = int (parts [0].strip (), 16)
        size  = int (parts [1].strip (), 16)
        if len (parts) > 2 :
            default = int (parts [2].strip (), 16)
        else :
            default = None
        for a in range (addr, addr + size) :
            if a in used_bytes :
                old = used_bytes [a]
                raise ValueError \
                    ( "Layout config invalid. "
                      "Address %s already used by %s and should "
                      "be used by %s as well"
                    % (a, old, o)
                    )
            used_bytes [a] = o
        layout [addr]  = o, size, default
    for hw in ini.sections () :
        if hw != "DEFINES" and hw != "CONFIG_SECTION" :
            data   = []
            offset = None
            for addr, (name, size, default) in sorted (layout.items ()) :
                value = ini.get (hw, name, fallback = default)
                if value is None :
                    raise ValueError \
                        ( "Value %s is required but not defined for variant %s"
                        % (name, hw)
                        )
                if isinstance (value, str) :
                    value = eval (value.lower (), defines, {})
                bin_data = struct.pack (size2code_map [size], value)
                if offset is not None :
                    pad = addr - offset
                    if pad :
                        data.append (b"\x00" * pad)
                data.append (bin_data)
            variants [hw] = b"".join (data)
    return variants
# end def read_variant_definition

def patch_bootloader (bl, name, data, location, dest_dir) :
    patched = bl.copy ()
    addr    = location
    for b in data :
        patched [addr] = b
        addr          += 1
    save_hex_file \
        (os.path.join (dest_dir, "bootloader-%s.hex" % name), patched)
# end def patch_bootloader

if __name__ == "__main__" :
    parser = argparse.ArgumentParser ()
    parser.add_argument ("bootloader",         type = str)
    parser.add_argument ("variant_definition", type = str, default = "patch_bootloader.ini")
    parser.add_argument ("-c", "--config-location",  type = int, default = 0x100)
    parser.add_argument ("-d", "--destination-dir",  type = str)
    cmd      = parser.parse_args       ()
    bl       = read_hex_file           (cmd.bootloader)
    variants = read_variant_definition (cmd.variant_definition)
    dest_dir = cmd.destination_dir
    if dest_dir is None :
        dest_dir = os.path.dirname (cmd.bootloader)
    save_hex_file (os.path.join (dest_dir, "bootloader-unpatched.hex"), bl.copy ())
    for name, data in variants.items () :
        patch_bootloader (bl, name, data, cmd.config_location, dest_dir)
