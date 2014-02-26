
/* List of Cells 
 * head: front cell
 * tail: back cell
 */ 
class List{

    //inner classes
    class Cell {
	public Node elm;
	public Cell next;

	public Cell(){
	}
    }

    Cell head = null;
    Cell tail = null;
    int elements = 0;

    Cell c = null;
    Node current = null;

    public void scan(List listNew){
	c = head;
	    while(c != null){
		current = c.elm;
		current.makeChildren();
		listNew.add3(current.child1, current.child2, current.child3);
		c = c.next;
	    }
	}

    public void add(Node elm){
	Cell c = new Cell();
	c.elm = elm;
	if(elements == 0){
	    head = c;
	    tail = c;
	    elements = 1;
	} else {
	    tail.next = c;
	    tail = tail.next;
	    elements++;
	}
    }
    
    public void add3(Node elm1, Node elm2, Node elm3){
	add(elm1);
	add(elm2);
	add(elm3);
    }
    
    public void makeEmpty(){
	elements = 0;
	tail = head;
    }
    
    public boolean isEmpty(){
	if(elements==0){
	    return true;
	} else {
	    return false;
	}
    }
}

class Node{
    Node child1,child2,child3;
    String id;
    boolean myMark;
    
    public Node(String str){
	id = str;
    }

    public Node changeChildN(int i, Node n){
	if (i == 1){
	    child1 = n;
	} else if(i == 2) {
	    child2 = n;
	} else if(i == 3) {
	    child3 = n;
	}
	return n;
    }

    public Node getChild(int i){
	if (i == 1){
	    return child1;
	} else if(i == 2) {
	    return child2;
	} else if(i == 3) {
	    return child3;
	}
	return null;
    }

    public void setId(String _id){
	id = _id;
    }

    public void printMe(Node n){
	if(n != null){
	    //System.out.println(id);
	    if(child1 != null){
		child1.printMe(child1);
	    }
	    if(child2 != null){
		child2.printMe(child2);
	    }
	    if(child3 != null){
		child3.printMe(child3);
	    }
	}
    }

    public void mark(){
	myMark = true;
    }

    public void unMark(){
	myMark = false;
    }

    public boolean isMarked(int i){
	Node n = getChild(i);
	boolean result;
	result = true;
	if(n != null){
	    result = n.myMark;
	} 
	return result;
    }

    public Node makeChildren() {
	child1 = new Node("a");
	child2 = new Node("b");
	child3 = new Node("c");
	return this;
    }
}

class Main{

    Node root, n1, n2, n3;

    public void dfs(Node n){
	n.mark();
	//n.printMe(n);
	for(int i = 1; i < 4; i++){
	    if(n.isMarked(i)){
		dfs(n.getChild(i));
	    }
	}
    }
    
    public Node makeTree(int depth){
	//List nodeList = new List();
	List listOld = new List();
	List listNew;
	Node root = new Node("root ");
	listOld.add(root);
	for(int i = 1; i < depth ; i++){
	    listNew = new List();
	    listOld.scan(listNew);
	    listOld.makeEmpty();
	    listOld = listNew;
	}
	return root;   
    }

    public void start(){
	root = makeTree(7);
	for(int i = 1; i<11; i++){
	    dfs(root);
	}
    }

    public static void main(String args[]){
	Main m = new Main();
	m.start();
    }
}




