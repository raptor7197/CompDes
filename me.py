# def read_file(filename):

#   try:
#       with open(filename, 'r') as file:
#           contents = file.read()
#         #   print(contents)
#           return contents
#   except FileNotFoundError:
#       print("File can't be found")
#       return None

# def add(content):
#     global reg
#     lines = content.split('\n')
#     for line in lines:
#       i = line.split(" ")
#       if i[0] == 'ADD':
#          reg+=int(i[1])
#     print(reg)


# if __name__ == "__main__":
#   reg = 10
#   filename = "cd.rxm"
#   file_content = read_file(filename)
#   add(file_content)

rega = 0 
rega=0
regm=1
regs=0
def read_file(filename):
  try:
      with open(filename, 'r') as file:
          file_content = file.read()
          return file_content
  except FileNotFoundError:
      print("File can't be found")

def add(file_content):
    global rega
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'ADD':
            try:
                rega += int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in ADD operation.")
    print(f"Result after ADD: {rega}")

def subtract(file_content):
    global regs
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'SUB':
            try:
                regs -= int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in SUB operation.")
    print(f"Result after SUB: {regs}")

def multiply(file_content):
    global regm
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'MUL':
            try:
                regm *= int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in MUL operation.")
    print(f"Result after MUL: {regm}")

def main():
    global reg
    file
    reg = 0  
    try:
      with open(filename, 'r') as file:
          file_content = file.read()
          return file_content
    except FileNotFoundError:
        print("File can't be found")

     
    add(file_content)
    subtract(file_content)
    multiply(file_content)

if __name__ == "__main__":
    
    file_content = read_file("cd.rxm")
    add(file_content)
    subtract(file_content)
    multiply(file_content)