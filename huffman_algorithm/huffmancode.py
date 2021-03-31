
class Node:
	def __init__(self,ch,freq,code):
		self.ch = ch
		self.freq = freq
		self.code = code
		self.left = None
		self.right = None
		
out = open("output.txt","w")
out.close()

fp = open("input.txt","r")
line = fp.readline()
fp.close()

# create a list of all the characters that appear
chars = []
for ch in line:
	if ch not in chars:
		chars.append(ch)

# create a list of the frequencies for each character that appears in line
freq_ls = []
for ch in chars:
	counter = 0
	for i in range(0,len(line)):
		if ch == line[i]:
			counter+=1
	freq_ls.append(counter) 


# create a list of node objects that hold the char and frequency values
forest = []
for i in range(0,len(freq_ls)):
	forest.append(Node(chars[i],freq_ls[i],""))

# gets the minimum frequency node
def get_min_node(ls):
	min_node = ls[0]
	for node in ls:
		if node.freq < min_node.freq:
			min_node = node
	return min_node

def huffman_code(forest): # forest is a list of nodes containing ch,freq as attributes
	
	while len(forest) > 1:

		# the two smallest frequencies
		left = get_min_node(forest)
		forest.remove(left)
		right = get_min_node(forest)
		forest.remove(right)


		# combine the two nodes and create a new one
		new_freq = left.freq + right.freq
		new_char = left.ch + right.ch
		combined_node = Node(new_char,new_freq,"")
		combined_node.right = left
		combined_node.left = right

		forest.append(combined_node)

		
	return forest[0] # one tree left in the forest, which is the huffman_code tree

huffman_code_tree = huffman_code(forest)

output_ls = []
def create_output(node,code):

	
	code_str = code + str(node.code) # concatenate the 1 or 0 based on the direction of current node

	# check if each left and right is an edge node
	if(node.left != None):
		create_output(node.left,code_str+"1")
	if(node.right != None):
		create_output(node.right,code_str+"0")

	# Node is an edge
	if(node.left == None and node.right == None):
		to_write = str(node.ch) + ":" + code_str
		output_ls.append(to_write)
		
	
create_output(forest[0]," ") # generates a list of all the things to write
output_file = open("output.txt","w")
for i in range(0,len(output_ls)):
	output_file.write(output_ls[i]+'\n')
output_file.close()




		


