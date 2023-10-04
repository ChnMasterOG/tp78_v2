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
                    start_pos = 0
                    while True:
                        p = data.find(folder_name[i] + '\\', start_pos)
                        print(p, folder_name[i] + '\\')
                        if p == -1:
                            break
                        start_pos = p + len(folder_name[i])
                        c_pos = data.rfind('"', 0, p)
                        s_pos = data.find(' ', start_pos)
                        data = data[:c_pos] + "\".." + data[start_pos:s_pos].replace('\\', '/') + data[s_pos:]
                        start_pos = c_pos
                    with open(fname, 'w') as fo:
                        fo.write(data)
