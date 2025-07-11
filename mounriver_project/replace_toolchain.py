import os

folder_name = ["KEYBOARD_CH582M", "RF_CH582M", "VLD_CH582M"]
fp_list = ["./" + i + "/obj" for i in folder_name]

for i in range(len(fp_list)):
    for dirpath, dirnames, filenames in os.walk(fp_list[i]):
        for filepath in filenames:
            fname = os.path.join(dirpath, filepath).replace('\\', '/')
            if fname[-9:] == "subdir.mk" or fname[-8:] == "makefile":
                print("file name: ", fname)
                with open(fname, "r") as fi:
                    data = fi.read()
                    data = data.replace("riscv-wch-elf", "riscv-none-elf")
                    with open(fname, 'w') as fo:
                        fo.write(data)
