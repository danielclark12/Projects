import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class HashTable<T> {

    private int size;
    int itemsInHash = 1;
    private NGen<T>[] hash;
    private int[] chainCounter;

    NGen node = new NGen();


     HashTable(int size){
        this.size = size;
        hash = (NGen<T>[]) new NGen[size];
        chainCounter = new int[size]; // Use an array to keep track of how many chains at each index
        for(int i = 0; i < size; i++){
            chainCounter[i] = 0;
        }
    }
    public void add(T item,int hashToUse){
         int index = 0;
         if(hashToUse == 1){
             index = hash(item); // hash function for unknown
         }
         else if(hashToUse == 2)
             index = hash2(item); // hash function for known
        else if(hashToUse == 3)
            index = hash3(item);
        else if(hashToUse == 4)
            index = hash4(item);

        if(index == -1)
            return;
        else{
            NGen newNode = new NGen();
            newNode.setData(item);
            if(hash[index] == null){ // Start a new linked list if there is nothing at the index
                hash[index] = newNode;
                chainCounter[index] = chainCounter[index] + 1; // keeps track of how many elements are at each index
            }
            else{
                boolean duplicate = false;

                NGen current = hash[index]; // Add onto the previous link to account for chaining
                if(current.getData().equals(item)) // Check if first item chained is a duplicate
                    return;
                NGen ptr = current.getNext();
                while(ptr != null){
                    current = ptr;
                    ptr = ptr.getNext();
                    if(current.getData().equals(item)){ // check for duplicate in the list
                        duplicate = true;
                        break;
                    }
                }
                if(!duplicate){
                    current.setNext(newNode);
                    chainCounter[index] = chainCounter[index] + 1; // keeps track of how many elements are at each index
                }
            }
        }
    }
    // Non specific hashtable
    private int hash(T key){
         if(key instanceof String){
             String keyString = (String) key;
             // convert to ASCII
             // Learned how to convert string into ascii from https://www.examtray.com/java/how-find-or-get-ascii-value-string-java
             int sum = 0;
             for(int i = 0; i < keyString.length(); i++){ // Get a total value for all ascii chars in the string
                 int asciiVal = keyString.charAt(i);
                 sum = sum + asciiVal;
             }
             int index = sum % hash.length;
             return index;
         }
         else return -1;
    }
    // Specific hash
    private int hash2(T key){ // This hash function is implemented using open addressing
         // The smallest the table size can be is the amount of words that are being added. This is because the function looks for the next open spot
        // if there are no open spots left when adding an item, it will be an infinite while loop
        if(key instanceof String){
            String keyString = (String) key;
            boolean added = false;
            int sum = 0;
            int counter = 1;

            for(int i = 0; i < keyString.length(); i++) {
                int asciiVal = keyString.charAt(i);
                sum = sum + asciiVal;
            }
            int index = sum % hash.length;
            if(hash[index] == null){
                return index;
            }
            else{
                while(!added){
                    // if sum + 1 % length not available, do sum + 2 % length and so forth
                    // idea for the formula came from here. https://www.geeksforgeeks.org/hashing-set-3-open-addressing/
                    index = (sum + counter) % hash.length; // Keep adding 1 until empty index is reached
                    if(hash[index] == null)
                        added = true;
                    counter++;
                }
                return index;
            }
        }
        else return -1;
    }
    
    // This hash function is the same as hash but it multiples the sum of the ascii chars by prime number 57. There are more open spaces but it has shorter chains
    private int hash3(T key){
        if(key instanceof String){
            String keyString = (String) key;
            // convert to ASCII
            // Learned how to convert string into ascii from https://www.examtray.com/java/how-find-or-get-ascii-value-string-java
            int sum = 0;
            for(int i = 0; i < keyString.length(); i++){ // Get a total value for all ascii chars in the string
                int asciiVal = keyString.charAt(i);
                sum = sum + asciiVal;
            }
            int index = sum * 51  % hash.length;
            return index;
        }
        else return -1;
    }

    private int hash4(T key){ // for known data without open addressing
        if(key instanceof String){
            String keyString = (String) key;
            boolean added = false;
            int sum = 0;
            int counter = 1;

            for(int i = 0; i < keyString.length(); i++) {
                int asciiVal = keyString.charAt(i);
                sum = sum + asciiVal;
            }
            int index = sum * itemsInHash * 47 % hash.length;
            itemsInHash++;
            return index;
            }

        else return -1;
    }


    public void display(){
         // Display All the index values
        System.out.print("| Index | " + "|   Elements   |\n");
         for(int i = 0; i < hash.length; i++) {
             if (i < 10)
                 System.out.print("|   " + i + "   |");
             else
                 System.out.print("|  " + i + "   |"); // if and else block used to help cleaner format
             // Display Each item for each index:
                 if (hash[i] == null) {
                     System.out.print("   ");
                 } else {
                     // Display everything in the linked list for each index
                     NGen current = hash[i];
                     current.setData(hash[i].getData());
                     System.out.print(" " + current.getData());
                     current = current.getNext();
                     while (current != null) {
                         System.out.print( " --> " + current.getData());
                         current = current.getNext();
                     }

                 }

             // Display the number of chains at each index:
             if(chainCounter[i] != 0)
                 System.out.print("    #chains = " + chainCounter[i] + "\n");
             else
                 System.out.println();

         }
    }
    // This method was written by The University Of Minnesota CSCI department. I modified it
    // to add the token to the hash and also to take in a filename instead of reading through the command line.
    public void readFile(String file, int hashToUse){
        Scanner readFile = new Scanner(System.in);
        String s;
        int count = 0;
        System.out.println();
        System.out.println("Attempting to read from file: " + file);
        try {
            readFile = new Scanner(new File(file));
        }
        catch (FileNotFoundException e) {
            System.out.println("File: " + file + " not found");
            System.exit(1);
        }

        System.out.println("Connection to file: " + file + " successful");
        System.out.println();
        while (readFile.hasNext()) {
            s = readFile.next();
            add((T)s,hashToUse); // My modified line
            count++;
        }
        System.out.println(count + " Tokens found");
    }

    public static void main(String[] args){

        HashTable table = new HashTable(100);
        HashTable table2 = new HashTable(100);
        HashTable table3 = new HashTable(50);
        HashTable keywords = new HashTable(50);
        table.readFile("gettysburg.txt",1); // Uses hash
        table.display();

        System.out.println("\nNew File:");
        table2.readFile("canterbury.txt",1); // Uses hash
        table2.display();

        System.out.println("Keywords File: This uses hash2");
        keywords.readFile("keywords.txt",2); // Uses hash2
        keywords.display();

        System.out.println("Hash 4: ");
        table3.readFile("keywords.txt",4); // Uses hash3
        table3.display();

    }
}
