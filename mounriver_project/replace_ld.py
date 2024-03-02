ld_file_name = "./KEYBOARD_CH582M/Ld/Link.ld"
kboot_start_addr = "0x00010000"

with open(ld_file_name, "r") as fi:
    data = fi.read()
    p = data.find(kboot_start_addr)
    if p == -1:
        print("no found kboot start addr!")
        raise Exception("No found kboot start addr")
    data = data.replace(kboot_start_addr, "0x00000000")
    with open(ld_file_name, 'w') as fo:
        fo.write(data)
