// ListItem.h: interface for the ListItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTITEM_H__03EFD781_D651_11D1_84DC_444553540000__INCLUDED_)
#define AFX_LISTITEM_H__03EFD781_D651_11D1_84DC_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class ListItem{
public:
    void *data;
    ListItem *next;
    ListItem *prev;
	ListItem(){
		next = 0;
		prev = 0;
		data = 0;
	};
	ListItem(void *dat){
		data = dat;
		next = 0;
		prev = 0;
	}
};

class List{
public:
	ListItem *list;
	long numelements;
	List()
	{
		Init();
	}
	void Init()
	{
		list = 0;
		numelements=0;
	}
//	~List(){Destroy();} 

bool List::Add(void *item){
        ListItem        *newnode, *curr;

        if((newnode = new ListItem()) == 0) {
//                Log("AddItem: Error allocating new list node\n");
                return false;
        }
	numelements++;
        if(this->list == 0) {
                /* Must construct a new list
                 * So set this to be the head, write it, and return true
                 */

                newnode->data = item;
                list = newnode;
				newnode->next = 0;
				newnode->prev = 0;
                return true;
        } else {
                /* Find the end of the list
                 * Check to see if we find the item we're about to insert
                 * If so, return false, because we don't want duplicate
                 * items in the list
                 */
                for(curr = list; curr->next!=0; curr=curr->next) {
                        if(curr->data == item)
                                return false;
                }

                newnode->data = item;
                curr->next = newnode;
                newnode->next = 0;
                newnode->prev = curr;
                return true;
        }
}
bool List::InList(void *item){//check to see if this item is already in the list
	bool retval = false;
		ListItem        *curr;
        for(curr= list; curr!=0; curr=curr->next) {
                if(curr->data == item) {
                        retval = true;
                        break;
                }
        }
	return retval;
}
bool List::Remove(void *item)
{
        ListItem        *curr, *node;
        int             inlist = false;

        /* Search through the list, to see if we can find the item we wish
         * to remove. If its not there, it can't be removed from the list
         * therefore we signal an error
         */

        for(curr= list; curr!=0; curr=curr->next) {
                if(curr->data == item) {
                        inlist = true;
                        node = curr;
                        break;
                }
        }

        if(!inlist) {
         //       printf("RemoveItem: Warning, item not found in list\n");
                return false;
        }

        /* Head of the list is a special case, we must deal with it
         * seperately
         */
	numelements--;
        if(node == list) {
                list = node->next;
                if(list != 0)
                    list->prev = 0;
				delete node;
                return true;
        } else {
                if(node->prev != 0)
                        node->prev->next = node->next;
                if(node->next != 0)
                        node->next->prev = node->prev;
				delete node;
                return true;
        }
}

/*
 * Function: DestroyList
 * Purpose : Release all the memory allocated to the nodes in the list
 * Input   : List
 * Output  : Nothing returned
 */

void List::Destroy(){
        ListItem        *node, *next;
        for(node= list; node!=0; node=next) {
                next = node->next;
                //free(node);
				delete node;
        }
		list = 0;//set the head of the list to be 0
		numelements = 0;
}

/*
 * Function: CountItems
 * Purpose : Count the number of items in the list
 * Input   : The list
 * Output  : Number of items
 */

int List::Count(){
//        ListItem        *node;
    //    int             count = 0;
  //      for(node= list; node!=0; node=node->next)
      //          count++;

        return (int)numelements;//count;
}

/*
 * Function: GetItem
 * Purpose : Get the item for a given index (zero-based)
 * Input   : List, index
 * Output  : Item pointer, 0 if not found
 */

void *List::GetItem(int index)
{
        ListItem        *node;

        /* Advance thru the list, checking to see if index == 0. We treat
         * index as a counter for "number of nodes to traverse to get to
         * the node we want to". So when its 0, its the head, when its 10,
         * its 10 nodes from the head, etc etc
         */

        for(node= list; node!=0; node=node->next) {
                if(index-- == 0)
                        return node->data;
        }

        return 0;
}

/*
 * Function: GetItemIndex
 * Purpose : Get an index, for an item. Return -1 on failure
 * Input   : Item pointer
 * Output  : Valid zero-based index if ok, else -1
 */

int List::GetItemIndex(void *item)
{
        ListItem        *node;
        int             index = 0;

        for(node= list; node!=0; node=node->next) {
                if(node->data == item)
                        return index;
                index++;
        }

        return -1;
}
};
#endif // !defined(AFX_LISTITEM_H__03EFD781_D651_11D1_84DC_444553540000__INCLUDED_)
