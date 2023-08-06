#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAXREGION 30
#define REGIONMAXSIZE 15
#define MAXCONTACT MAXREGION - 1

typedef struct s_Region{
    char* name;
    struct s_Region** RegionTouch;
    int PIB;
    char** temp;


    //Solution /////////
    int ValuePIB;
    int team;
    int valided;
    //Parsing /////////

} t_Region;



//Solution /////////////////////////////////////

typedef struct s_result{
    t_Region*   region1;
    t_Region*   region2;
    int value;
} t_Result;

float Moyenne;
t_Region **AllRegion;
float EcartType;
int NumberInEcartType;
t_Result **ListContact;
t_Result **finalResult;

float moy(t_Region ** region, float number){
    float somme = 0;
    for (int i = 0; region[i]; i++){
        somme += region[i]->PIB;
    }
    return (somme / number);
}

void definePIB(){
    for (int i = 0; AllRegion[i]; i++){
        AllRegion[i]->ValuePIB = (AllRegion[i]->PIB - Moyenne) * (AllRegion[i]->PIB - Moyenne);
    }
}

float ecarType(t_Result **myResult){
    NumberInEcartType++;
    float ret = 0;
    for(int i = 0; myResult[i]; i++){
        if (myResult[i]->value){

            //dprintf(1, " %s-%s ", myResult[i]->region1->name, myResult[i]->region2->name);
            //dprintf(1, "%d-%d ", myResult[i]->region1->team, myResult[i]->region2->team);

            if (!myResult[i]->region1->valided){
                int moyennedif = -Moyenne;
                for (int j = i; myResult[j]; j++){
                    if (myResult[j]->value && myResult[j]->region1->team == myResult[i]->region1->team){
                        if (!myResult[j]->region1->valided){
                            moyennedif += myResult[j]->region1->PIB;
                            myResult[j]->region1->valided = 1;
                        }

                        if (!myResult[j]->region2->valided){
                            moyennedif += myResult[j]->region2->PIB;
                            myResult[j]->region2->valided = 1;
                        }
                    }
                }
                ret += (moyennedif * moyennedif);

                //dprintf(1, "-%d ", moyennedif * moyennedif);
            }
        }
    }
    for (int i = 0; AllRegion[i]; i++){
        if (AllRegion[i]->valided == 0){

            //dprintf(1, " %d-", AllRegion[i]->ValuePIB);
            //dprintf(1, " %s ", AllRegion[i]->name);

            ret += AllRegion[i]->ValuePIB;
        }
        else
            AllRegion[i]->valided = 0;
    }

    //dprintf(1, "resultat %f \n\n", ret / 10);

    return ret;
}

void search(int PathNumber, int i){
    if (PathNumber == 0)
    {
        float temp = ecarType(ListContact);
        if (temp < EcartType)
        {
            EcartType = temp;
            int j = 0;
            for (int i = 0; ListContact[i] != NULL; i++){
                if (ListContact[i]->value == 1)
                { 
                    finalResult[j++] = ListContact[i];
                }        
            }
        }
        return ;
    }
    int memTeam[MAXREGION];
    int myTeam;
    int j;
    int k;
    for (; ListContact[i]; i++){
        if (ListContact[i]->region1->team != ListContact[i]->region2->team){
            myTeam = ListContact[i]->region2->team;
            k = 0;
            for (j = 0; AllRegion[j]; j++){
                if (AllRegion[j]->team == myTeam){
                    AllRegion[j]->team = ListContact[i]->region1->team;

                    //dprintf(1, " convert %s to %s ", AllRegion[j]->name, ListContact[i]->region1->name);

                    memTeam[k] = j;
                    k++;
                }
            }
            ListContact[i]->value = 1;
            search( PathNumber - 1, i + 1);
            k--;
            ListContact[i]->value = 0;
            for (; k >= 0; k--){
                AllRegion[memTeam[k]]->team = myTeam;
            }
        }
    }
}

int lenRegion(t_Region** myReg){
    int i;
    for ( i = 0; myReg[i]; i++){}
    return i;
}

t_Result **listAllContact(t_Region**Region){
    AllRegion = Region;
    int len = 0;
    for (int i = 0; Region[i]; i++){
        for(int j = 0; Region[i]->RegionTouch[j]; j++){
            if (Region[i]->RegionTouch[j] > Region[i])
                len++;
        }
    }
    t_Result **ret = malloc(sizeof(t_Result *) * len + 1);
    ret[len] = NULL;
    len = 0;
    for (int i = 0; Region[i]; i++){
        for(int j = 0; Region[i]->RegionTouch[j]; j++){
            if (Region[i]->RegionTouch[j] > Region[i]){
                ret[len] = malloc(sizeof (t_Result));
                ret[len]->region1 = Region[i];
                ret[len]->region2 = Region[i]->RegionTouch[j];
                ret[len]->value = 0;
                len++;
            }
        }
    }
    return ret;
}

void convert(int i, int newTeam){
    finalResult[i]->region1->team = newTeam;
    finalResult[i]->region2->team = newTeam;
    for (int j = 0; finalResult[j]; j++){
        if (finalResult[j]->region1 == finalResult[i]->region1 && finalResult[i]->region1->team != finalResult[j]->region2->team)
            convert(j, newTeam);
        if (finalResult[j]->region2 == finalResult[i]->region1 && finalResult[i]->region1->team != finalResult[j]->region1->team)
            convert(j, newTeam);
        if (finalResult[j]->region1 == finalResult[i]->region2 && finalResult[i]->region2->team != finalResult[j]->region2->team)
            convert(j, newTeam);
        if (finalResult[j]->region2 == finalResult[i]->region2 && finalResult[i]->region2->team != finalResult[j]->region1->team)
            convert(j, newTeam);
    }
}

void convertToTeam(){
    int j = 0;
    for (int i = 0; AllRegion[i]; i++){
        AllRegion[i]->team = -1;
    }

    for (int i = 0; finalResult[i]; i++){
        if (finalResult[i]->region1->team == -1 && finalResult[i]->region2->team == -1){
            finalResult[i]->region1->team = j;
            finalResult[i]->region2->team = j;
            convert(i, j);
            j++;
        }
        else {
            if (finalResult[i]->region1->team == -1)
                finalResult[i]->region1->team = finalResult[i]->region2->team;
            if (finalResult[i]->region2->team == -1)
                finalResult[i]->region2->team = finalResult[i]->region1->team;
        }
    }
    
    for (int i = 0;AllRegion[i]; i++){
        if (AllRegion[i]->team == -1){
            AllRegion[i]->team = j;
            j++;
        }
    }
}

char ***parsingToTheEnd(){
    char ***ret = malloc(sizeof(char **) * (MAXREGION + 1));
    int lenRet = -1;
    for (int i = 0; AllRegion[i]; i++){
        if (AllRegion[i]->valided == 0){
            lenRet++;
            int lenMalloc = 1;
            AllRegion[i]->valided = 1;
            for (int k = i; AllRegion[k]; k++){
                if (AllRegion[k]->team == AllRegion[i]->team){
                    lenMalloc++;
                    AllRegion[k]->valided = 1;
                }
            }
            ret[lenRet] = malloc(sizeof (char * ) * lenMalloc);
            int l = 0;
            for (int m = i; AllRegion[m]; m++){
                if (AllRegion[m]->team == AllRegion[i]->team){
                    ret[lenRet][l] = AllRegion[m]->name;
                    l++;
                }
            }
            ret[lenRet][l] = NULL;
        }
    }
    return (ret);
}

void freeExec(){
    for (int i = 0; ListContact[i]; i++){
        free(ListContact[i]);
    }
    free(ListContact);
    free(finalResult);
}

char ***algo(t_Region **result, int NumberRegion){

    if (NumberRegion > lenRegion(result)){
        dprintf(1, "Nombre de regions teste trop grand\n");
        return (NULL);
    }

    Moyenne = moy(result, NumberRegion);
    EcartType = __FLT_MAX__;

    dprintf(1, "La moyenne est %f\n", Moyenne);
    
    NumberInEcartType = 0;
    int maxNum = lenRegion(result) - NumberRegion + 1;

    dprintf(1, "Le nombre de liens sera de %d\n\n", maxNum - 1);
    
    finalResult = malloc(sizeof (t_Result) * ( maxNum));
    finalResult[maxNum - 1] = NULL;

    for (int i = 0; result[i]; i++){
        result[i]->team = i;
    }
    
    ListContact = listAllContact(result);
    definePIB();
    dprintf(1, "Les contacts sont :");
    
    t_Region *lastContact = NULL;
    int a = 0;
    for (;ListContact[a]; a++){
        if (ListContact[a]->region1 != lastContact){
            dprintf(1, "\n");
            lastContact = ListContact[a]->region1;
        }
        dprintf(1, " [%s-%s]", ListContact[a]->region1->name, ListContact[a]->region2->name);
    }
    dprintf(1, "\n\nLeur nombre est de %d\n\n", a);

    search(maxNum - 1,0);

    dprintf(1, "Les contacts pris sont :");

    lastContact = NULL;
    for (int i = 0; finalResult[i]; i++){
        if (ListContact[i]->region1 != lastContact){
            dprintf(1, "\n");
            lastContact = ListContact[i]->region1;
        }
        dprintf(1, " [%s-%s]",finalResult[i]->region1->name, finalResult[i]->region2->name );
    }

    dprintf(1, "\n\nNombre total de cas teste : %d\n", NumberInEcartType);
    dprintf(1, "\nL'ecart-type final est : %f\n", EcartType);

    convertToTeam();
    char ***ret = parsingToTheEnd();
    freeExec();

    return ret;
}

char** bigSplit(char *str, char delim){
  
	if (str == NULL)
		return (NULL);

	int sizeMalloc = 0;
	int count = 0;
	int _bool = 0;
	while (str[sizeMalloc])
	{
		if (str[sizeMalloc] != delim && !_bool)
		{
			count++;
			_bool = 1;
		}
		else if (str[sizeMalloc] == delim && _bool)
			_bool = 0;
		sizeMalloc++;
	}

	char	**bigarray = (char **)malloc(sizeof(char *) * (count + 1));
	if (!bigarray)
		return (NULL);

    int	countStr = 0;
    int i = 0;

	while (str[i]){
		if (str[i] == delim)
			i++;
		else{
    	    int		j = 0;
            while (str[i + j] && str[i + j] != delim){
                j++;
            }
            bigarray[countStr] = (char *)malloc(sizeof(char) * (j + 1));
            if (!bigarray[countStr]){
				while (countStr > 0){
		            free(bigarray[countStr - 1]);
		            countStr--;
	            }
				return (NULL);
			}
            j = 0;
            while (str[i + j] && str[i + j] != delim){
                bigarray[countStr][j] = str[i + j];
                j++;
            }
            bigarray[countStr][j] = '\0';
            i = i + j;
			
			countStr++;
		}
	}
	bigarray[count] = 0;
	return (bigarray);
}

t_Region** freeRegion(t_Region** myRegion, int i){
    if (i >= 0){

        if (myRegion[i]->temp){
            for (int j = 0; myRegion[i]->temp[j]; j++)
                free(myRegion[i]->temp[j]);
            free(myRegion[i]->temp);
        }
        if (myRegion[i]-> name)
            free(myRegion[i]->name);
        free(myRegion[i]);
        myRegion[i] = NULL;
    }
    for (int i = 0; myRegion[i]; i++){
        if (myRegion[i]->temp){
            for (int j = 0; myRegion[i]->temp[j]; j++)
                free(myRegion[i]->temp[j]);
            free(myRegion[i]->temp);
        }
        free(myRegion[i]->name);
        if (myRegion[i]->RegionTouch)
            free(myRegion[i]->RegionTouch);
        free(myRegion[i]);
    }
    free(myRegion);
    return NULL;
}

t_Region** parseFile(FILE* fd){
    char bufferName[REGIONMAXSIZE];
    char bufferList[REGIONMAXSIZE * MAXCONTACT + MAXCONTACT - 1];
    int bufferPIB[1];
    t_Region **myRegion;
    myRegion = malloc(sizeof (t_Region*) * MAXREGION);
    if (!myRegion)
        return myRegion;
    int i = 0;
    int result = 3;
    while (result == 3 && i < MAXREGION){
        result = fscanf(fd,"%s : %d : %s\n", bufferName, bufferPIB, bufferList);
        if (result == 3){
            //printf("Read Name |%s| PIB = |%d| ListofName = |%s| Result = |%d| \n", bufferName, *bufferPIB, bufferList,result); 
            myRegion[i] = malloc(sizeof(t_Region));
            if (myRegion[i] == NULL)
                return freeRegion(myRegion, -1);
            myRegion[i]->temp = bigSplit(bufferList, '-');
            myRegion[i]->PIB = *bufferPIB;
            myRegion[i]->name = strdup(bufferName);
            if (!myRegion[i]->temp || !myRegion[i]-> name)
                return freeRegion(myRegion, i);
        }
        i++;
    }
    myRegion[--i] = NULL;
    if (result != -1){
        printf("Erreur dans le parsing\n");
        return freeRegion(myRegion, -1);
    }
    return myRegion;
}

int pointerStrlen(char **str){
    int i = 0;
    while (str[i]){
        i++;
    }
    return i;
}

t_Region* searchByName(t_Region** myRegion, char * _name){
    for (int i = 0; myRegion[i]; i++){
        if (strcmp(myRegion[i]->name, _name) == 0)
            return (myRegion[i]);
    }
    dprintf(1, "Error, Region %s not find\n", _name);
    return NULL;
}

t_Region** putPointer(t_Region** MyRegion){
    for (int i = 0; MyRegion[i] != NULL; i++){
        int size = pointerStrlen(MyRegion[i]->temp);
        MyRegion[i]->RegionTouch = malloc(sizeof (t_Region) * size);
        if (MyRegion[i]->RegionTouch == NULL)
            return freeRegion(MyRegion, -1);
        MyRegion[i]->RegionTouch[--size] = NULL;
        for (; size >= 0; size--){
            MyRegion[i]->RegionTouch[size] = searchByName(MyRegion, MyRegion[i]->temp[size]);
            
            if (MyRegion[i]->RegionTouch[size] == NULL){

                return freeRegion(MyRegion, -1);
            }
        }
    }
    return MyRegion;
}

void freePointerStr(char ***str){
    for (int i = 0; str[i]; i++){
        free(str[i]);
    }
    free(str);
}

void printRet(char ***ret, FILE * fdRet){
    for (int i = 0; ret[i]; i++){
		for(int j = 0; ret[i][j]; j++){
			fprintf(fdRet, "%s", ret[i][j]);
			if (ret[i][j + 1])
				fprintf(fdRet, "-");
		}
		fprintf(fdRet, "\n");
	}
}


int main(int argc, char **argv){
    if (argc != 4)
        return printf("Erreur : il n'y a pas le bon nombre d'arguments\n");
    
    FILE* fd = fopen(argv[1], "r");
    if (fd == NULL)
        return printf("Le fichier de region n'a pas ete trouve\n");
    FILE * fdRet = fopen(argv[2], "w");
    if (fdRet == NULL)
        return printf("impossible d'ouvrir le fichier d'ecriture\n");

	int NumberRegion = atoi(argv[3]);
	if (NumberRegion < 1 || NumberRegion > MAXREGION)
		return printf("le nombre de regions souhaites n'est pas valide\n");

    t_Region** result = parseFile(fd);
    if (result == NULL)
        return 1;
    result = putPointer(result);
    if (result == NULL)
        return 1;
    
    char ***ret = algo(result, NumberRegion);

    if (ret == NULL)
		return(1);
	printRet(ret,  fdRet);
	freeRegion(result, -1);
    freePointerStr(ret);
    return 0;
}