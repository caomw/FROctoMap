#include "CFremenGrid.h"

using namespace std;

CFremenGrid::CFremenGrid(int size)
{
	signalLength = 0;
	numCells = size;
	order = 0;
	cellArray = (CFrelement**) malloc(numCells*sizeof(CFrelement*));
	for (int i=0;i<numCells;i++) cellArray[i] = new CFrelement();
	plan = new CFFTPlan();
}

CFremenGrid::~CFremenGrid()
{
	for (int i=0;i<numCells;i++) free(cellArray[i]);
	delete plan;
	free(cellArray);
}

void CFremenGrid::setName(const char* n)
{
	strcpy(name,n);
}

void CFremenGrid::setPose(float x, float y)
{
	positionX = x;
	positionY = y;
}

void CFremenGrid::update(int order,int signalLengthi)
{
	signalLength = signalLengthi;
	plan->prepare(signalLength);
	cout << "Signal length " << signalLength << " of " << cellArray[0]->getLength() << endl;
	for (int i=0;i<numCells;i++){
//		if (i%100==0)cout << "Updating cell " << i << " of " << numCells << endl;
		cellArray[i]->update(order,plan);
	}
}

void CFremenGrid::save(const char* filename,bool lossy)
{
	FILE* f=fopen(filename,"w");
	unsigned char nameLen = strlen(name);
	fwrite(&nameLen,sizeof(unsigned char),1,f);
	fwrite(name,nameLen,1,f);
	fwrite(&xDim,sizeof(int),1,f);
	fwrite(&yDim,sizeof(int),1,f);
	fwrite(&zDim,sizeof(int),1,f);
	fwrite(&positionX,sizeof(float),1,f);
	fwrite(&positionY,sizeof(float),1,f);

	fwrite(&numCells,sizeof(int),1,f);
	fwrite(&signalLength,sizeof(int),1,f);
	for (int i=0;i<numCells;i++) cellArray[i]->save(f,lossy);
	fclose(f);
}

bool CFremenGrid::load(const char* filename)
{
	int ret = 0;
	signalLength = 0;
	FILE* f=fopen(filename,"r");
	if (f == NULL){
		printf("FrOctomap %s not found, aborting load.\n",filename);
		return false;
	}
	printf("Loading FrOctomap %s.\n",filename);
	for (int i=0;i<numCells;i++){
		 free(cellArray[i]);
		 //fprintf(stdout,"Cells %i %ld %d\n",i,sizeof(CFrelement*),signalLength);
	}
	free(cellArray);
	unsigned char nameLen = 0;
	ret = fread(&nameLen,sizeof(unsigned char),1,f);
	ret = fread(name,nameLen,1,f);
	name[nameLen] = 0;
	ret = fread(&xDim,sizeof(int),1,f);
	ret = fread(&yDim,sizeof(int),1,f);
	ret = fread(&zDim,sizeof(int),1,f);
	ret = fread(&positionX,sizeof(float),1,f);
	ret = fread(&positionY,sizeof(float),1,f);
	ret = fread(&numCells,sizeof(unsigned int),1,f);
	ret = fread(&signalLength,sizeof(unsigned int),1,f);
//	fprintf(stdout,"Cells %i, signal length %i\n",numCells,signalLength);
	cellArray = (CFrelement**) malloc(numCells*sizeof(CFrelement*));
	for (int i=0;i<numCells;i++) cellArray[i] = new CFrelement();
	for (int i=0;i<numCells;i++){
		cellArray[i]->load(f);
		cellArray[i]->signalLength = signalLength;
	}
	printf("FrOctomap %s loaded: name %s with %i cells and %i observations.\n",filename,name,numCells,signalLength);
	fclose(f);
	return true;
}

void CFremenGrid::print(int number)
{
	if (cellArray[number]->order > 0 || cellArray[number]->outliers > 0){
		printf("Cell: %i ",number);
		cellArray[number]->print();
	}
}

void CFremenGrid::reconstruct(int number,unsigned char *reconstructed)
{
	cellArray[number]->reconstruct(reconstructed,plan);
}

void CFremenGrid::add(unsigned int index,unsigned char value)
{
	cellArray[index]->add(value);
}

unsigned char CFremenGrid::retrieve(unsigned int index,unsigned int timeStamp)
{
	return cellArray[index]->retrieve(timeStamp);
}

float CFremenGrid::estimate(unsigned int index,unsigned int timeStamp)
{
	return cellArray[index]->estimate(timeStamp);
}

float CFremenGrid::fineEstimate(unsigned int index,float timeStamp)
{
	return cellArray[index]->fineEstimate(timeStamp);
}
