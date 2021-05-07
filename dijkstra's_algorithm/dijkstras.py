
# global var to represnt our max val
INFINITY = 2000000

# get vertex with the minimum distance
def get_min(queue,dist,num_verticies):

    minimum = 2000000   
    min_index = -1
    
    for index in range(num_verticies):  # get the min value from the distance list
        if dist[index] < minimum and index in queue:
            minimum = dist[index]
            min_index = index

    return min_index # location of where the minimum occurs

# relax algorithm from lecture
def relax(u,i,graph,q,distance,shortest_path):
	if graph[u][i] and i in q:
		if distance[i] > distance[u] + graph[u][i]:
			distance[i] = distance[u] + graph[u][i]
			shortest_path[i] = u

def dijkstra(graph, start, end, num_verticies):

	# distance will be 0 since its not going to travel so just write straight to output
	out_first = ''
	if start == end:
		out_first =  "0: " + str(end)
		out = open("output.txt","w")
		out.write(out_first)
		out.close()
		return out_first

	# assumption of 2000000 as max val
	distance = [INFINITY] * num_verticies 

	shortest_path = [-99] * num_verticies

	# size of each list
	num_per_list = graph[0]
	
	# no distance has to be traveled since its itself
	distance[start] = 0

	q = [] # add all the verticies to the q as shown in Q = G.V
	for key in graph.keys():
		q.append(key)

	# follow pseudocode from lecture
	while len(q) != 0:

		u = get_min(q,distance,num_verticies)
		
		for i in range(len(graph[0])): # for each v in G.Adj[u]
			relax(u,i,graph,q,distance,shortest_path)
		
		q.remove(u)

	# we now have a total distance traveled
	total_distance = distance[end]
	
	# get the path of the visited nodes
	visited = []
	while True:
		if shortest_path[end] == -99:
			break
		else:
			visited.append(end)
		end = shortest_path[end]

	# add the final node
	visited.append(end)
	
	visited.reverse()
	write(str(total_distance),visited)

def write(distance,visited):
	 out_string = distance + ":"
	 for i in visited:
	 	out_string += " " + str(i) 

	 out = open("output.txt","w")
	 out.write(out_string)
	 out.close()

# put each line in file into its own list in a list to simulate a matrix row
with open('input.txt', 'r') as f:
    matrix = [[int(num) for num in line.split(" ")] for line in f]


# get the starting and destination vectors
source = matrix[0][0]
destination = matrix[0][1]

# remove the starting and destination vectors from overall matrix
matrix.remove(matrix[0])

mat_graph = {}

# create a dictionary of our matrix
for i in range(len(matrix)):
    # each key corresponds to a row: EX: 0 : [0, 200000, 4]
    mat_graph[i] = []
    for j in range(len(matrix[i])):
        mat_graph[i].append(matrix[i][j])

verticies = len(matrix)
dijkstra(mat_graph,source,destination, verticies)