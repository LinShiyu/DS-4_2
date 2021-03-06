// DS exp4_2.cpp: 定义控制台应用程序的入口点。
//

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

//频度表的节点结构 
typedef struct {
	int value; //权重
	char cha; //字符
}code, *pcode;

//哈夫曼树结点结构
typedef struct {
	int weight;//结点权重
	char charact;//节点对应字符
	int parent, left, right;//父结点、左孩子、右孩子在数组中的位置下标
}HTNode, *HuffmanTree;

//动态二维数组，存储哈夫曼编码
typedef char ** HuffmanCode;

//end表示HT数组中存放结点的最终位置，s1和s2传递的是HT数组中权重值最小的两个结点在数组中的位置
void Select(HuffmanTree HT, int end, int *s1, int *s2)
{
	int min1, min2;
	//遍历数组初始下标为 1
	int i = 1;
	//找到还没构建树的结点
	while (HT[i].parent != 0 && i <= end) { //当前节点的双亲结点不为0(不是散的)，并且没有完，向后遍历
		i++;
	}
	min1 = HT[i].weight; //找到的第一个散的节点，值赋给min1，下标赋给s1
	*s1 = i;

	i++; //向后跳过找到的第一个散的节点，继续找第二个
	while (HT[i].parent != 0 && i <= end) {
		i++;
	}
	//对找到的两个结点比较大小，min2为大的，min1为小的
	if (HT[i].weight < min1) {
		min2 = min1;
		*s2 = *s1;
		min1 = HT[i].weight;
		*s1 = i;
	}
	else {
		min2 = HT[i].weight;
		*s2 = i;
	}
	//两个结点和后续的所有未构建成树的结点做比较
	for (int j = i + 1; j <= end; j++) {
		//如果有父结点，直接跳过，进行下一个
		if (HT[j].parent != 0) {
			continue;
		}
		//如果比最小的还小，将min2=min1，min1赋值新的结点的下标
		if (HT[j].weight < min1) {
			min2 = min1;
			min1 = HT[j].weight;
			*s2 = *s1;
			*s1 = j;
		}
		//如果介于两者之间，min2赋值为新的结点的位置下标
		else if (HT[j].weight >= min1 && HT[j].weight < min2) {
			min2 = HT[j].weight;
			*s2 = j;
		}
	}
}

void CreateHuffmanTree(HuffmanTree *HT, int *w, char *ch, int n)
{
	if (n <= 1) {
		return; // 如果只有一个编码就相当于0
	}
	int m = 2 * n - 1; // 哈夫曼树总节点数，n就是叶子结点
	*HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
	HuffmanTree p = *HT;
	// 初始化哈夫曼树中的所有结点
	for (int i = 1; i <= n; i++) { // 0号位置不用
		(p + i)->weight = *(w + i - 1);
		(p + i)->charact = *(ch + i - 1);
		(p + i)->parent = 0;
		(p + i)->left = 0;
		(p + i)->right = 0;
	}
	//从树组的下标 n+1 开始初始化哈夫曼树中除叶子结点外的结点
	for (int i = n + 1; i <= m; i++) {
		(p + i)->weight = 0;
		(p + i)->parent = 0;
		(p + i)->left = 0;
		(p + i)->right = 0;
	}
	//构建哈夫曼树
	for (int i = n + 1; i <= m; i++) {
		int s1, s2;
		Select(*HT, i - 1, &s1, &s2); //第一次共有n个，第二次n+1个...
		(*HT)[s1].parent = (*HT)[s2].parent = i; //将最小的两个节点赋上同样的双亲节点
		(*HT)[i].left = s1; //左节点最小，右节点其次
		(*HT)[i].right = s2;
		(*HT)[i].weight = (*HT)[s1].weight + (*HT)[s2].weight; //创建的双亲结点值为两个子节点的和
	}
}

void HuffmanCoding(HuffmanTree HT, HuffmanCode *HC, int n)
{
	*HC = (HuffmanCode)malloc((n + 1) * sizeof(char *)); //最终存放哈夫曼编码的数组
	char *cd = (char *)malloc(n * sizeof(char)); //临时存放结点哈夫曼编码的字符串数组
	cd[n - 1] = '\0';//字符串结束符

	//第一次运行时生成1号节点的编码，第二次运行...2号节点...直到w中节点都完成编码
	for (int i = 1; i <= n; i++) {
		//从叶子结点出发，得到的哈夫曼编码是逆序的，需要在字符串数组中逆序存放
		int start = n - 1;
		//当前结点在数组中的位置
		int c = i;
		//当前结点的父结点在数组中的位置
		int j = HT[i].parent;
		// 一直寻找到根结点
		while (j != 0) {
			// 如果该结点是父结点的左孩子则对应路径编码为0，否则为右孩子编码为1
			if (HT[j].left == c) {
				start--;
				cd[start] = '0';
			}
			else {
				start--;
				cd[start] = '1';
			}
			//以父结点为孩子结点，继续朝树根的方向遍历
			c = j;
			j = HT[j].parent;
		}
		//跳出循环后，cd数组中从下标 start 开始，存放的就是该结点的哈夫曼编码
		(*HC)[i] = (char *)malloc((n - start) * sizeof(char));
		strcpy((*HC)[i], &cd[start]); //复制从start起直到\0的部分
	}
	//使用malloc申请的cd动态数组需要手动释放
	free(cd);
}

void HuffmanDeCoding(HuffmanTree HT, HuffmanCode *HC, char *decode_char, int n)
{
	printf("Decoding for each code...\n");
	int m = 2 * n - 1;
	int pos = m; //现在的位置,初始为根节点
	for (int i = 1; i <= n; i++) {
		int j = 0;
		while (pos > 0) {
			if ((*HC)[i][j] == '0') {
				printf("0");
				pos = HT[pos].left; //现在位置变为现在位置的左子节点的位置
				j++;
			}
			else if ((*HC)[i][j] == '1') {
				printf("1");
				pos = HT[pos].right; //现在位置变为现在位置的右子节点的位置
				j++;
			}
			else {
				printf("... %c", HT[pos].charact); //输出当前解码的字符
				decode_char[i] = HT[pos].charact; //将当前解码字符存在decode_char[]中
				pos = m; //找完一个节点的位置后，重置pos到根节点位置
				break;
			}
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 1;i <= 28;i++) {
		printf("%c", decode_char[i]);
	}
	printf("\n");
}

void PrintHuffmanCode(HuffmanCode htable, int *w, char *ch, int n)
{
	printf("Huffman code for each letter...\n");
	for (int i = 1; i <= n; i++) {
		printf("%c(%d).... %s\n", ch[i - 1], w[i - 1], htable[i]);
	}
	printf("\n");
	for (int i = 1;i <= n;i++) {
		printf("%s ", htable[i]);
	}
	printf("\n\n");
}

void init_table(pcode table)
{
	table[0].cha = ' ';
	table[0].value = 186;
	table[1].cha = 'A';
	table[1].value = 64;
	table[2].cha = 'B';
	table[2].value = 13;
	table[3].cha = 'C';
	table[3].value = 22;
	table[4].cha = 'D';
	table[4].value = 32;
	table[5].cha = 'E';
	table[5].value = 103;
	table[6].cha = 'F';
	table[6].value = 21;
	table[7].cha = 'G';
	table[7].value = 15;
	table[8].cha = 'H';
	table[8].value = 47;
	table[9].cha = 'I';
	table[9].value = 57;
	table[10].cha = 'J';
	table[10].value = 1;
	table[11].cha = 'K';
	table[11].value = 5;
	table[12].cha = 'L';
	table[12].value = 32;
	table[13].cha = 'M';
	table[13].value = 20;
	table[14].cha = 'N';
	table[14].value = 57;
	table[15].cha = 'O';
	table[15].value = 63;
	table[16].cha = 'P';
	table[16].value = 15;
	table[17].cha = 'Q';
	table[17].value = 1;
	table[18].cha = 'R';
	table[18].value = 48;
	table[19].cha = 'S';
	table[19].value = 51;
	table[20].cha = 'T';
	table[20].value = 80;
	table[21].cha = 'U';
	table[21].value = 23;
	table[22].cha = 'V';
	table[22].value = 8;
	table[23].cha = 'W';
	table[23].value = 18;
	table[24].cha = 'X';
	table[24].value = 1;
	table[25].cha = 'Y';
	table[25].value = 16;
	table[26].cha = 'Z';
	table[26].value = 1;
}

//遍历str，将里面字符的频度依次保存在w中
void find_freq(char *str, pcode table, int *w)
{
	for (int i = 0;str[i] != '\0';i++) {
		for (int j = 0;j < 27;j++) {
			if (table[j].cha == str[i]) {
				w[i] = table[j].value;
			}
		}
	}
	for (int k = 0; k < 27; k++) {
		printf("%d ", w[k]);
	}
	printf("\n\n");
}

int main()
{
	char str[] = "THIS PROGRAM IS MY FAVORITE";
	puts(str);

	code table[27]; //定义长度为27（26+1）的table类型数组
	init_table(table);
	int w[27]; //定义长度为str长度-1的数组w,存放str中字符的频度

	find_freq(str, table, w);

	int n = 27;
	HuffmanTree htree;
	HuffmanCode htable;
	CreateHuffmanTree(&htree, w, str, n);
	HuffmanCoding(htree, &htable, n);
	PrintHuffmanCode(htable, w, str, n);

	char decode_char[28];
	HuffmanDeCoding(htree, &htable, decode_char, n);

	return 0;
}
