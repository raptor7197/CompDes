reg = 0
def read_file(filename):
  try:
      with open(filename, 'r') as file:
          file_content = file.read()
        #   print(contents)
          return file_content
  except FileNotFoundError:
      print("File can't be found")

def add(file_content):
    global reg
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'ADD':
            try:
                reg += int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in ADD operation.")
    print(f"Result after ADD: {reg}")

def subtract(file_content):
    global reg
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'SUB':
            try:
                reg -= int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in SUB operation.")
    print(f"Result after SUB: {reg}")

def multiply(file_content):
    global reg
    lines = file_content.split('\n')
    for line in lines:
        i = line.split(" ")
        if i[0] == 'MUL':
            try:
                reg *= int(i[1])
            except ValueError:
                print(f"Error: Invalid number '{i[1]}' in MUL operation.")
    print(f"Result after MUL: {reg}")

def main():
    global reg
    file
    reg = 0  
    try:
      with open(filename, 'r') as file:
          file_content = file.read()
        #   print(contents)
          return file_content
    except FileNotFoundError:
        print("File can't be found")

    # add_input =   
    # subtract_input = "SUB 3\nSUB 2\nSUB xyz"  
    # multiply_input = "MUL 4\nMUL 2\nMUL -1"  
    add(file_content)
    subtract(file_content)
    multiply(file_content)

if __name__ == "__main__":
    # main()
    # read_file("cd.rxm")
    # filename = "cd.rxm"
    file_content = read_file("cd.rxm")
    add(file_content)