#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "header.h"

double get_distance(Node_t * nodes, int idx1, int idx2);
int    get_min(Node_t * nodes, ListNode * list_heap);
void   append_element(Node_t *, ListNode * list_heap, int idx);

#define ORDER <

bool read_cord(char * file_name, ListNode * a_node) {
    int num_node = 0, num_edge = 0, i;
    Node_t * nodes = NULL;

    bool isvalid;
    FILE * fp;
    if ((isvalid = ((fp = fopen(file_name, "r")) != NULL))) {
        isvalid = fscanf(fp, "%d %d", &num_node, &num_edge) == 2;

        // store nodes in an array
        size_t alloc_size = 0;
        if (isvalid && 
           (isvalid = (nodes = malloc((alloc_size = (num_node * sizeof(*nodes))))) 
                       != NULL)) {
            memset(nodes, 0, alloc_size);
            for(i = 0; i < num_node && isvalid; i++) {
                int x, y, idx;
                isvalid = fscanf(fp, "%d %d %d", &idx, &x, &y) == 3;
                nodes[idx] = (Node_t) {.x = x, .y = y};
            }
            if (!isvalid) {
                free(nodes);
            }
        }
        *a_node = (ListNode) {.heap = nodes, .size = num_node};

        // store the edge as linked list
        if (isvalid) {
            for (i = 0; i < num_edge; i++) {
                int idx, tempidx;
                isvalid = (fscanf(fp, "%d %d", &idx, &tempidx) == 2);
                if (!isvalid) {
                    free(nodes);
                }
                else {
                    llong_t temp = nodes[idx].adj_head;
                    if (temp.idx == NULL) {
                        temp = (llong_t) {.idx = malloc(10 * sizeof(int)), .size = 10, .i = 0};
                    }
                    else if ((temp.i + 1) >= temp.size) {
                        temp.size = temp.size + 10;
                        temp.idx  = realloc(temp.idx, temp.size);
                    }
                    temp.idx[temp.i] = tempidx;
                    temp.i = temp.i + 1;
                    nodes[idx].adj_head = temp;
                }
            }
        }

        // check non adjacent
        int j, k;
        if (isvalid) {
            for (i = 0; i < num_node; i++) {
                llong_t temp = nodes[i].adj_head;
                for (j = 0; j < temp.i; j++){
                    llong_t temptemp = nodes[temp.idx[j]].adj_head;
                    bool not_found   = true;
                    for (k = 0; 
                         k < temptemp.i && (not_found = (temptemp.idx[k] != i)); 
                         k++) {}
                        
                    if (not_found) {
                        if (temptemp.idx == NULL) {
                            temptemp = (llong_t) {.idx = malloc(10 * sizeof(int)), .size = 10, .i = 0};
                        }
                        else if ((temptemp.i + 1) >= temptemp.size) {
                            temptemp.size = temptemp.size + 10;
                            temptemp.idx  = realloc(temptemp.idx, temptemp.size);
                        }
                        temptemp.idx[temptemp.i] = i;
                        temptemp.i = temptemp.i + 1;
                        nodes[temp.idx[j]].adj_head = temptemp;
                    }
                }
            }
        }
        fclose(fp);
    }

    return isvalid;
}

void function(dijkstra_t dijkstra, int adj_idx) {
    int new_distance = get_distance(dijkstra.nodes, dijkstra.curr_idx, adj_idx) + dijkstra.nodes[dijkstra.curr_idx].distance;
    if (adj_idx != dijkstra.curr_pidx) {
        if ((dijkstra.prev[adj_idx] == -1) ||
            (dijkstra.nodes[adj_idx].distance > new_distance)) {
            dijkstra.nodes[adj_idx].distance = new_distance;
            append_element(dijkstra.nodes, dijkstra.heap, adj_idx);
            dijkstra.prev[adj_idx] = dijkstra.curr_idx;
        }
    }
}

bool dijkstra(int node1, int node2, ListNode list_node, ListNode * list_heap, int ** a_prev) {
    size_t alloc_size;
    Node_t * nodes = list_node.heap;

    if (list_heap->heap == NULL) {
        *list_heap = (ListNode) {.heap = malloc((alloc_size = (100 * sizeof(int)))), .size = 100};
        memset(list_heap->heap, 0, alloc_size);
    }
    list_heap->idx = 0;

    int * prev;
    if (*a_prev == NULL) {
        prev = malloc(list_node.size * sizeof(*prev));
        *a_prev = prev;
    }
    else {
        prev = *a_prev;
    }
    int i;
    for (i = 0; i < list_node.size; i++) {
        prev[i] = -1;
    }

    bool not_found        = true;
    nodes[node1].distance = 0;
    dijkstra_t dijkstra   = {.nodes = nodes, .heap = list_heap, .prev = prev};
    append_element(nodes, list_heap, node1);
    while (not_found) {
        dijkstra.curr_idx  = get_min(nodes, list_heap);
        dijkstra.curr_pidx = prev[dijkstra.curr_idx];

        if ((not_found = (dijkstra.curr_idx != node2))) {
            llong_t temp = nodes[dijkstra.curr_idx].adj_head;
            int i;
            for (i = 0; i < temp.i; i++) {
                function(dijkstra, temp.idx[i]);
            }
        }
        not_found = list_heap->idx != 0 && not_found;
    }
    
    return dijkstra.curr_idx == node2;
}

void append_element(Node_t * nodes, ListNode * list_heap, int idx) {
    int * heap     = list_heap->heap;

    int curr_idx   = -1;
	int i;
	for (i = 0; (curr_idx == -1) && (i < list_heap->idx); i++) {
		if (heap[i] == idx) {
			curr_idx = i;
		}
	}
    if (curr_idx < 0) {
        // realloc memory
        curr_idx = list_heap->idx;
        if ((curr_idx + 1) >= list_heap->size) {
            list_heap->size = list_heap->size + 10;
            heap = realloc(heap, sizeof(*heap) * list_heap->size);
            list_heap->heap = heap;
        }
        heap[curr_idx] = idx;
        list_heap->idx = list_heap->idx + 1;

    }

    int temp, node_idx = -1;
    for (;curr_idx > 0 && 
          nodes[heap[(node_idx = ((curr_idx - 1) / 2))]].distance > nodes[heap[curr_idx]].distance; curr_idx = node_idx) {
        // swap
        temp           = heap[node_idx];
        heap[node_idx] = heap[curr_idx];
        heap[curr_idx] = temp;
    }
}

int get_min(Node_t * nodes, ListNode * list_heap) {
    int * heap     = list_heap->heap;
    int size       = list_heap->idx - 1;

    int min           = heap[0];
    heap[0]           = heap[size];
    list_heap->idx    = size;

    int temp, idx;
    int left_dist, right_dist;
    int left_idx, right_idx, i = 0;
    bool is_left      = true;
    bool not_finished = size > 1;
    for (;not_finished; i = is_left ? left_idx: right_idx) {
        // check values
        left_idx  = (2 * i) + 1;
        right_idx = left_idx + 1;
        if ((not_finished = (left_idx < size))) {
            if (right_idx > size) {
                right_idx = left_idx;
            }

            left_dist  = nodes[heap[left_idx]].distance;
            right_dist = nodes[heap[right_idx]].distance;

            if ((not_finished = ((nodes[heap[i]].distance > left_dist || nodes[heap[i]].distance > right_dist) && left_idx < size))) {
                // swap
                is_left = (left_dist < right_dist);
                temp    = heap[i];
                heap[i] = is_left ? heap[left_idx]: heap[right_idx];
                heap[(idx = (is_left ? left_idx: right_idx))] = temp;
            }
        }
    }

    return min;
}

void free_nodes(Node_t * nodes, int size) {
    int i;
    for (i = 0; i < size; i++) {
        llong_t curr = nodes[i].adj_head;
        free(curr.idx);
    }
    free(nodes);
}

double get_distance(Node_t * nodes, int idx1, int idx2) {
    long x_diff = nodes[idx1].x - nodes[idx2].x;
    long y_diff = nodes[idx1].y - nodes[idx2].y;

    double dist_square = (x_diff * x_diff) + (y_diff * y_diff);
    return (sqrt(dist_square));
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
