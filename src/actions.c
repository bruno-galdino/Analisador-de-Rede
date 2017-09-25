
#include "../include/actions.h"


void RemoveNodeRandomly(Network *network)
{
    
    if( !network->number_of_nodes )
        return;
    
    srand((unsigned)time(NULL));
    
    int rand_number = rand() % ( network->number_of_nodes);
    
    DestroyNode(network,rand_number);
    
}



void RemoveArcRandomly(Network *network)
{
    if( !network->number_of_arcs)
        return;
    
    srand((unsigned) time(NULL));
    
    int rand_number = rand() % ( network->number_of_arcs);
    
    DestroyArc(network,rand_number);
    
}



void RemoveN_NodesRandomly(Network * network,float n)
{
    
    int nodes_to_remove = 0;
    int i = 0;
    int rand_number;
    
    if( n > 0.0f && n < 1.1f )// trata n percentual de nos a serem removidos
    {
        
        nodes_to_remove = network->number_of_nodes * n;
        
    }        
    
    else if ( n > 0.0f && fabsf(n - network->number_of_nodes) <= DIFERENCE )// trata n como sendo a quantide de nos a serem removidos
    {
        
        nodes_to_remove = n;
        
    }
    
    srand( (unsigned)time(NULL));
    
    while( i < nodes_to_remove)
    {
        
        rand_number = rand() %  network->number_of_nodes;
        
        DestroyNode(network,rand_number);
        
        i++;
    }
    
    
}



void RemoveN_ArcsRandomly(Network * network,float n)
{
    
    int arcs_to_remove = 0;
    int i = 0;
    int rand_number;
    
    if( n > 0.0f && n < 1.0f )// trata n percentual de arcos a serem removidos
    {
        
        arcs_to_remove = network->number_of_arcs * n;
        
    }        
    
    else if ( n > 0.0f && fabsf(n - network->number_of_arcs) <= DIFERENCE )// trata n como sendo a quantide de arcos a serem removidos
    {
        
        arcs_to_remove = n;
        
    }
    
    srand( (unsigned)time(NULL));
    
    while( i < arcs_to_remove)
    {
        
        rand_number = rand() %  network->number_of_arcs;
        
        DestroyArc(network,rand_number);
        
        i++;
    }
    
}



void VisitNodes(Network *network)
{
    
    bool visited_nodes[network->number_of_nodes]; 
    int *matrix = GetAdjacenceMatrix(network);
    int i;
    
    for(i = 0 ; i < network->number_of_nodes; i++)
    {
        visited_nodes[i] = false;
        strcpy( network->nodes[i]->connected_to , network->nodes[i]->name );
        network->nodes[i]->on_line = false;
    }
    
    for(i = 0; i < network->number_of_nodes;i++)
    {    

        if( network->nodes[i]->node_type == GENERATION_UNIT)
        {
            Visit(network,matrix,visited_nodes,i,i,network->nodes[i]->name);
        
        }           
    }
    
    FreeAdjacenceMatrix(matrix);
}



bool UserInput( Network **network, Network **copy)
{
    
    if(key[KEY_N])
    {
        RemoveNodeRandomly(*network);
        
        key[KEY_N]  = 0;
        
        return false;
    }   
    
    if(key[KEY_A])
    {
       
        RemoveArcRandomly(*network);
        
        key[KEY_A]  = 0;
        
        return false;
    }
    
    if( key[KEY_C] )
    {
        
        DestroyNetwork(*copy);
        
        *copy = CopyNetwork(*network);
        
        if( !(*copy) )
        {
            allegro_message("Nao foi possivel copiar a rede");
            
        }
        
        key[KEY_C] = 0;
        
        return false;
        
    }
    
    if( key[KEY_R] )
    {
    
        DestroyNetwork(*network);
        
        *network = CopyNetwork(*copy);
            
        if( !(*network) )
        {
            allegro_message("Nao foi possivel restaurar a rede");
        
        }
        
        key[KEY_R] = 0 ;
        
        return false;
        
    }
    
    if(key[KEY_ESC])
    {
       
        return true;
    }
    
}



void SaveStats(Network *network, const char *file_name,float percent)
{
    
    static bool first_call = true;
    
    
    FILE *file = fopen(file_name,"a");
    
    if( !file )
    {
        
        perror("Nao foi possivel abrir arquivo");
        return;
        
    }
    
    int components = NumberOfConnectedComponents(network);
    float inverse_components;
    
    if( components )
    {
        inverse_components = 1/ ( ( float)components );
    }
    else 
    {
        inverse_components = components;
    }
    
    
    if( first_call )
    {
        fprintf(file,"%%,     Nos,    Arcos,  Usinas,  Componentes,  Componentes ^ -1,   Maior Componente,   Nos desconectados,\n");
        first_call = false;
    }
    
        fprintf(
                file,
                "%3d,   %4d,   %4d,     %3d,     %4d,           %0.2f,               %4d,                %4d,\n",
                (int)(percent*100),
                network->number_of_nodes,
                network->number_of_arcs,
                network->number_of_generation_units,
                components,
                inverse_components,
                LargestConnectedComponentSize(network),
                NumberOfOffLineNodes(network)
                );
    
    fclose(file);
    
}



void MakeEnsemble(Network **network,float ensemble_from, float ensemble_to, const char *out_file)
{
    float i;
    Network *copy = NULL;
    
    for( i = ensemble_from ; i <= ensemble_to ; i += 0.01)
    {
    
        DestroyNetwork(copy);
        
        copy = CopyNetwork(*network);
        
        if( !copy )
        {
            allegro_message("Nao foi possivel fazer o Ensembly estatistico");
            return;
        }
        
        RemoveN_ArcsRandomly(copy,i);
        
        VisitNodes(copy);
        
        SaveStats(copy,out_file,i);
        
    }
    
    if( copy )
    {
        DestroyNetwork( *network );
    
        *network = CopyNetwork(copy);
    }
    
}