/*问题描述：
给出n个学生的m门课程的成绩表，每个学生的信息由学号、姓名以及各科成绩组成。对学生的考试成绩进行有关统计分析，并打印统计表。
基本要求：
1、	通过键盘输入各学生的多门课程的成绩，建立相应的文件input.dat。
2、	对文件input.dat中的数据进行处理，要求具有如下功能：
i.	按各门课程成绩排序，并生成相应的文件输出。
ii.	计算每人的平均成绩，按平均成绩排序，并生成文件。
iii.求出各门课程的平均成绩、最高分、最低分、不及格人数、60~69分人数、70~79分人数、80~89分人数、90分以上人数。
iv.	根据姓名或学号查询某人的各门课成绩，重名情况也能处理。
3、	界面美观。
4、 选做内容  对各科成绩设置不同的权值。
*/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#pragma warning(disable:4996)

#define MAX 100

/*学生的类型定义*/
struct STUDENT	
{
	int studentNo; //学号
	char name[20]; //姓名
	float scores[3];//各科成绩
	float ravg_score; //加权后的平均成绩
	float avg_score; //不加权的平均成绩
};


/*------------------模块1 输入、获取平均分-------------------------------------*/

/*计算加权的总分和不加权的总分*/
/*同时设置浮点型数组ratios来存储各科所占权值的比率,n表示学生人数,m表示m门课程*/
void getAvgScore(STUDENT students[],float ratios[], int n,int m)
{
	int i, j;
	float sum = 0, rsum = 0;
	for  (i= 0; i < n; i++)
	{
		sum = 0;
		rsum = 0;
		for ( j = 0; j < m; j++)
		{
			sum += students[i].scores[j];
			rsum += students[i].scores[j] * ratios[j];
		}
		students[i].avg_score = sum / m;
		students[i].ravg_score = rsum;
	}
}

/*键盘输入各学生的多门课程的成绩*/
/*参数n表示学生人数*/
void input(STUDENT students[], int n,int m)
{
	printf("                    ——————————————————————————————\n");
	int i,j;
	printf("                    请输入学生的学号、姓名、1-数学成绩、2-英语成绩、3-计算机成绩\n");
	printf("                    ——————————————————————————————\n");
	for (i = 0; i < n; i++)
	{
		printf("                    输入第%d个学生的信息：\n",i+1);
		printf("                    学号：\n                    ");
		scanf("%d", &students[i].studentNo);
		printf("                    姓名：\n                    ");
		scanf("%s", &students[i].name);
		for ( j = 0; j < m; j++)
		{
			printf("                    第%d科成绩：\n                    ", j + 1);
			scanf("%f", &students[i].scores[j]);
		}
		printf("                    ——————————————————————————————\n");
	}
}

void input_ratios(float rat[])
{
	int i;
	FILE *fp;
	fp = fopen("ratios.data", "wb");
	if (fp == NULL)
	{
		printf("Can't open the file");
	}
	printf("\n\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
	printf("                    请输入各科权重（和为1且大于0小于1）：\n\n");
	while (1)
	{
		printf("                    数学成绩的权重为：\n                    ");
		scanf("%f", &rat[0]);
		if (rat[0] >= 0 && rat[0] <= 1)
		{
			break;
		}
		else
		{
			printf("                    数学成绩权重输入有误！\n\n");
		}
	}
	while (1)
	{
		printf("\n                    英语成绩的权重为：\n                    ");
		scanf("%f", &rat[1]);
		if (rat[1] >= 0 && rat[1] <= 1 && (rat[1] + rat[0]) <= 1)
		{
			break;
		}
		else
		{
			printf("                    英语成绩权重输入有误！\n\n");
		}
	}
	rat[2] = 1.0 - rat[0] - rat[1];
	for (i = 0; i < 3; i++)
	{
		fwrite(&rat[i], sizeof(float), 1, fp);
		//写入权重
	}
	fclose(fp);
	printf("\n                    则计算机成绩的权重为：                    \n                    %.2f\n\n", rat[2]);
	printf("                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
}

/*--------------------------------------------------------------------------------*/



/*------------------模块2 文件读取、写入-------------------------------------*/

/*数据存入文件*/
void write(STUDENT students[], int n, const char f_name[])
{
	int i;
	FILE *fp;
	fp = fopen(f_name, "wb");
	if (fp == NULL)
	{
		printf("Can't open the write file");
	}
	//printf("Can't open the ");
	for (i = 0; i < n; i++)
	{
		fwrite(&students[i], sizeof(struct STUDENT), 1, fp);
		//一次写入一个学生的成绩
	}
	fclose(fp);
}

/*从文件读出数据*/
void read(STUDENT students[], int n,const char f_name[])//
{
	int i;
	FILE *fp;
	fp = fopen(f_name, "rb");
	if (fp == NULL)
	{
		printf("Can't open read the file！");
	}
	for ( i = 0; i < n; i++)
	{
		fread(&students[i], sizeof(struct STUDENT), 1, fp);
	}
}



/*------------------模块3 排序-------------------------------------*/

/*采用快速排序按各门课程进行排序*/
/*k表示科目规定：1--数学成绩，2--英语，3--计算机,4--平均成绩，5--加权平均成绩，6--学号*/
void exchange(STUDENT *a,STUDENT *b)
{
    STUDENT temp;
	temp = *a;
	*a = *b;
	*b = temp;
}


int Quick_One_Pass(STUDENT stu[], int startIndex, int endIndex,int k)
{
	int left = startIndex;
	//把第一个元素作为基准元素
	STUDENT &pivot = stu[startIndex];
	int right = endIndex;
	while (left != right)
	{
		/*比较各科目成绩*/
		if (k >= 1 && k <= 3)
		{
			//控制right右移动
			while (left < right && stu[right].scores[k - 1] <= pivot.scores[k - 1])
			{
				right--;
			}
			//left左移
			while (left < right && stu[left].scores[k - 1] >= pivot.scores[k - 1])
			{
				left++;
			}
		}

		/*比较平均成绩*/
		if (k == 4)
		{
			//控制right右移动
			while (left < right && stu[right].avg_score <= pivot.avg_score)
			{
				right--;
			}
			//left左移
			while (left < right && stu[left].avg_score >= pivot.avg_score)
			{
				left++;
			}
		}

		/*比较加权平均成绩*/
		if (k == 5)
		{
			//控制right右移动
			while (left < right && stu[right].ravg_score <= pivot.ravg_score)
			{
				right--;
			}
			//left左移
			while (left < right && stu[left].ravg_score <= pivot.ravg_score)
			{
				left++;
			}
		}

		/*比较学号*/
		if (k == 6)
		{
			//控制right右移动
			while (left < right && stu[right].studentNo > pivot.studentNo)
			{
				right--;
			}
			//left左移
			while (left < right && stu[left].studentNo > pivot.studentNo)
			{
				left++;
			}
		}

		//交换left和right所在位置的值
		if (left < right)
		{
			exchange(&stu[left], &stu[right]);
		}
	}
	//当left=right 交换基准元素和该位置（left或者right）的值
	exchange(&stu[left], &pivot);
	return left;
}


void Quick_Sort(STUDENT stu[], int startIndex, int endIndex,int k)
{
	if (startIndex >= endIndex)
	{
		return;
	}
	//pivotIndex：基准元素
	int pivotIndex = Quick_One_Pass(stu, startIndex, endIndex,k);
	//对基准元素前进行递归
	Quick_Sort(stu, startIndex, pivotIndex - 1,k);
	//对基准元素后进行递归
	Quick_Sort(stu, pivotIndex + 1, endIndex,k);
}

/*--------------------------------------------------------------------------------*/






/*------------------模块4 对各门成绩进行分析-------------------------------------*/

int degree(float score)
{
	if (score < 60)
	{
		return 0;
	}
	else if(score >= 60 && score < 70)
	{
		return 1;
	}
	else if (score >= 70 && score < 80)
	{
		return 2;
	}
	else if (score >= 80 && score < 90)
	{
		return 3;
	}
	else if (score >= 90)
	{
		return 4;
	}
}

//将结果写入文件中
void write_analyse_result_score(float result[], const char f_name[])
{
	int i;
	FILE *fp;
	int n = 8;
	fp = fopen(f_name, "wb");
	if (fp == NULL)
	{
		printf("Can't open the write file");
	}
	for (i = 0; i < n; i++)
	{
		fwrite(&result[i], sizeof(float), 1, fp);
		//一次写入一个类目的结果
	}
	fclose(fp);
}
//将结果读出
//k==1->math,k==2->english,k==3->cs 
void read_analyse_result_score( const char f_name[],int k)
{
	int i;
	float result[8];
	FILE *fp;
	int n = 8;
	fp = fopen(f_name, "rb");
	if (fp == NULL)
	{
		printf("Can't open the file！");
	}
	for (i = 0; i < n; i++)
	{
		fread(&result[i], sizeof(float), 1, fp);
	}
	if (k == 1)
	{
		printf("\n\n                    -----------------------------数学------------------------------");
	}
	else if (k == 2)
	{
		printf("\n\n                    -----------------------------英语------------------------------");
	}
	else if (k == 3)
	{
		printf("\n\n                    ----------------------------计算机-----------------------------");
	}
	printf("\n\n                    平均分\t不及格\t60~69\t70~79\t80~89\t90~100\t最高分\t最低分\n");
	printf("                    ");
	for (i = 0; i < 8; i++)
	{
		printf("%.1f\t", result[i]);
	}
}

//最低or最高学生的成绩
void write_analyse_result_student(STUDENT student, const char f_name[])
{
	int i;
	FILE *fp;
	fp = fopen(f_name, "wb");
	if (fp == NULL)
	{
		printf("Can't open the file");
	}
	//printf("Can't open the ");
	fwrite(&student, sizeof(struct STUDENT), 1, fp);
	//写入最低or最高学生的成绩
	fclose(fp);
}
//k==1->max,k==2->min
void read_analyse_result_student(const char f_name[],int k)
{
	int i;
	FILE *fp;
	STUDENT student;
	fp = fopen(f_name, "rb");
	if (fp == NULL)
	{
		printf("Can't open the file！");
	}
	fread(&student, sizeof(struct STUDENT), 1, fp);
	if (k == 1)
	{
		printf("\n\n                    该科成绩最高的学生为：\n");
		printf("                    学号：\t%d\t姓名：\t%s\n\n", student.studentNo, student.name);
	}
	else if (k == 2)
	{
		printf("\n                    该科成绩最低的学生为：\n");
		printf("                    学号：\t%d\t姓名：\t%s\n\n", student.studentNo, student.name);
		printf("                    ---------------------------------------------------------------\n");
	}

}

void read_all_result()
{
	printf("\n\n                    ======================== 各科统计结果==========================");
	read_analyse_result_score("math_result.data", 1);
	read_analyse_result_student("math_stu_max.data", 1);
	read_analyse_result_student("math_stu_min.data", 2);
	read_analyse_result_score("english_result.data", 2);
	read_analyse_result_student("english_result.data", 1);
	read_analyse_result_student("english_result.data", 2);
	read_analyse_result_score("cs_result.data", 3);
	read_analyse_result_student("cs_result.data", 1);
	read_analyse_result_student("cs_result.data", 2);
	printf("\n\n                    ===============================================================\n\n");
}

/*求出各门课程的平均成绩、最高分、最低分、不及格人数、60~69分人数、70~79分人数、80~89分人数、90分以上人数*/
/*n为学生人数*/
void data_analyse(STUDENT students[],int n)
{
	float math_result[8], english_result[8], cs_result[8];
	STUDENT stu_max[3], stu_min[3];
	int i;
	for ( i = 0; i < 8; i++)
	{
		math_result[i] = 0;
		english_result[i] = 0;
		cs_result[i] = 0;
	}
	for (i = 0; i < n; i++)
	{
		//每科总成绩
		math_result[0] += students[i].scores[0];
		english_result[0] += students[i].scores[1];
		cs_result[0] += students[i].scores[2];

		//分数段统计
		//数学
		switch (degree(students[i].scores[0]))
		{
		case 0:
			math_result[1]++;
			break;
		case 1:
			math_result[2]++;
			break;
		case 2:
			math_result[3]++;
			break;
		case 3:
			math_result[4]++;
			break;
		case 4:
			math_result[5]++;
		}

		//英语
		switch (degree(students[i].scores[1]))
		{
		case 1:
			english_result[2]++;
			break;
		case 2:
			english_result[3]++;
			break;
		case 3:
			english_result[4]++;
			break;
		case 4:
			english_result[5]++;
			break;
		}
		//计算机
		switch (degree(students[i].scores[2]))
		{
		case 0:
			cs_result[1]++;
			break;
		case 1:
			cs_result[2]++;
			break;
		case 2:
			cs_result[3]++;
			break;
		case 3:
			cs_result[4]++;
			break;
		case 4:
			cs_result[5]++;
			break;
		}
	}
	//各门课统计最高分、最低分

	//数学
	Quick_Sort(students, 0, n, 1);
	stu_max[0] = students[0];
	stu_min[0] = students[n - 1];
	math_result[6] = stu_max[0].scores[0];
	math_result[7] = stu_min[0].scores[0];
	 
	//英语
	Quick_Sort(students, 0, n, 2);
	stu_max[1] = students[0];
	stu_min[1] = students[n - 1];
	english_result[6] = stu_max[1].scores[1];
	english_result[7] = stu_min[1].scores[1];

	//计算机
	Quick_Sort(students, 0, n, 3);
	stu_max[2] = students[0];
	stu_min[2] = students[n - 1];
	cs_result[6] = stu_max[2].scores[2];
	cs_result[7] = stu_min[2].scores[2];

	//平均分
	math_result[0] = math_result[0] / n;
	english_result[0] = english_result[0] / n;
	cs_result[0] = cs_result[0] / n;

	write_analyse_result_score(math_result, "math_result.data");
	write_analyse_result_student(stu_max[0], "math_stu_max.data");
	write_analyse_result_student(stu_min[0], "math_stu_min.data");

	write_analyse_result_score(english_result, "english_result.data");
	write_analyse_result_student(stu_max[1], "english_stu_max.data");
	write_analyse_result_student(stu_min[1], "english_stu_min.data");

	write_analyse_result_score(cs_result, "cs_result.data");
	write_analyse_result_student(stu_max[2], "cs_stu_max.data");
	write_analyse_result_student(stu_min[2], "cs_stu_min.data");

	printf("\n\n                    ======================== 各科统计结果==========================");
	printf("\n\n                    -----------------------------数学------------------------------");
	printf("\n\n                    平均分\t不及格\t60~69\t70~79\t80~89\t90~100\t最高分\t最低分\n");
	printf("                    ");
	for ( i = 0; i < 8; i++)
	{
		printf("%.1f\t", math_result[i]);
	}
	printf("\n\n                    该科成绩最高的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n", stu_max[0].studentNo, stu_max[0].name);
	printf("\n                    该科成绩最低的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n\n", stu_min[0].studentNo, stu_min[0].name);
	printf("                    ---------------------------------------------------------------\n");

	printf("\n\n                    -----------------------------英语------------------------------");
	printf("\n\n                    平均分\t不及格\t60~69\t70~79\t80~89\t90~100\t最高分\t最低分\n");
	printf("                    ");
	for (i = 0; i < 8; i++)
	{
		printf("%.1f\t", english_result[i]);
	}
	printf("\n\n                    该科成绩最高的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n", stu_max[1].studentNo, stu_max[1].name);
	printf("\n                    该科成绩最低的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n\n", stu_min[1].studentNo, stu_min[1].name);
	printf("                    ---------------------------------------------------------------\n");

	printf("\n\n                    ----------------------------计算机-----------------------------");
	printf("\n\n                    平均分\t不及格\t60~69\t70~79\t80~89\t90~100\t最高分\t最低分\n");
	printf("                    ");
	for (i = 0; i < 8; i++)
	{
		printf("%.1f\t", cs_result[i]);
	}
	printf("\n\n                    该科成绩最高的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n", stu_max[2].studentNo, stu_max[2].name);
	printf("\n                    该科成绩最低的学生为：\n");
	printf("                    学号：\t%d\t姓名：\t%s\n\n", stu_min[2].studentNo, stu_min[2].name);
	printf("                    ---------------------------------------------------------------\n");
}

/*--------------------------------------------------------------------------------*/




/*------------------模块5 查找-------------------------------------*/
//二分法查找
STUDENT Binary_search(STUDENT stu[], int length, int key)
{
	int low, high, mid;
	STUDENT stu_null;
	stu_null.studentNo = -1;
	low = 0;
	high = length - 1;
	while (low <= high)
	{
		mid = (low + high) / 2;
		if (stu[mid].studentNo == key)
		{
			return stu[mid];
		}
		if (stu[mid].studentNo > key)
		{
			high = mid - 1;
		}
		if (stu[mid].studentNo < key)
		{
			low = mid + 1;
		}
	}
	return stu_null;
}
//根据学号查询某人的各门课成绩
//n表示学生数量
void search_Sno(STUDENT stu[],int n)
{
	int no;
	STUDENT stu_result;
	Quick_Sort(stu, 0, n-1, 6);
	printf("\n                    ---------------------------------------------\n");
	printf("\n                    请输入要查询同学的学号：\n");
	printf("\n                    ---------------------------------------------\n                    ");
	scanf("%d", &no);
	system("cls");
	stu_result = Binary_search(stu, n, no);
	if (stu_result.studentNo == -1)
	{
		printf("                    \n该学生信息不存在！请重新查找！\n");
	}
	else
	{
		printf("\n                    -----------------------------------学生信息表--------------------------------------------\n\n");
		printf("                    学号\t姓名\t数学成绩\t英语成绩\t计算机成绩\t平均分\t\t加权平均分\n");
		printf("                    %d\t\t%s\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", 
			stu_result.studentNo, stu_result.name, stu_result.scores[0],
			stu_result.scores[1], stu_result.scores[2], stu_result.avg_score, stu_result.ravg_score);
		printf("\n                    -----------------------------------------------------------------------------------------\n");
	}
}

/*根据姓名查询某人的各门课成绩，重名情况也能处理。*/
void search_Name(STUDENT stu[], int n)
{
	char name[20];
	int i,flag = 0, count = 0;
	printf("\n                    ---------------------------------------------\n");
	printf("\n                    请输入要查询同学的姓名：\n");
	printf("\n                    ---------------------------------------------\n                    ");
	scanf("%s", &name);
	system("cls");
	printf("\n                    -----------------------------------------------学生信息表--------------------------------------------------------\n\n");
	for ( i = 0; i < n; i++)
	{
		if (strcmp(name,stu[i].name) == 0)
		{
			flag = 1;
			count++;
			printf("                    第 %d 个学生:\n\n                    学号：%d\t姓名：%s\t数学成绩：%.1f\t英语成绩：%.1f\t计算机成绩：%.1f\t平均分：%.1f\t加权平均分%.1f\n",
				count,stu[i].studentNo, stu[i].name, stu[i].scores[0],
				stu[i].scores[1], stu[i].scores[2], stu[i].avg_score, stu[i].ravg_score);
		}
	}
	if (flag == 0)
	{
		printf("                    查找的学生不存在！");
	}
	printf("\n                    -----------------------------------------------------------------------------------------------------------------\n");
}

/*--------------------------------------------------------------------------------*/


/*------------------模块6 输出-------------------------------------*/
void output(STUDENT students[], int n)
{
	printf("\n                    -----------------------------------学生信息表-------------------------------------------------\n");
	printf("                    学号\t姓名\t数学成绩\t英语成绩\t计算机成绩\t平均分\t\t加权平均分\n");
	for (int i = 0; i < n; i++)
	{
		printf("                    %d\t\t%s\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", students[i].studentNo, students[i].name, students[i].scores[0],
			students[i].scores[1], students[i].scores[2], students[i].avg_score, students[i].ravg_score);
	}
	printf("                    ----------------------------------------------------------------------------------------------\n");
}

/*--------------------------------------------------------------------------------*/

/*------------------模块7 添加学生-------------------------------------*/

//追加写入添加学生
void add(STUDENT students[], int n, const char f_name[])
{
	int i;
	FILE *fp;
	fp = fopen(f_name, "ab");
	if (fp == NULL)
	{
		printf("Can't open the file");
	}
	//printf("Can't open the ");
	for (i = 0; i < n; i++)
	{
		fwrite(&students[i], sizeof(struct STUDENT), 1, fp);
		//一次写入一个学生的成绩
	}
	fclose(fp);
}

//n表示文件现有学生人数
int add_student(int n)
{
	int m;
	STUDENT stu[MAX];
	float ratios[3];
	int i;
	FILE *fp;
	fp = fopen("ratios.data", "rb");
	if (fp == NULL)
	{
		printf("Can't open the file");
	}
	for (i = 0; i < 3; i++)
	{
		fread(&ratios[i], sizeof(float), 1, fp);
	}
	printf("\n\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	printf("                    $请输入新增学生人数(小于100)：                     $\n                    ");
	scanf("%d", &m);
	printf("\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
	input(stu,m,3);
	getAvgScore(stu, ratios, m, 3);
	add(stu, m, "input.data");
	n = m + n;
	return n;
}


/*------------------模块8 界面-------------------------------------*/
void first_menu()
{
	int account, pass;
	printf("\n\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	printf("\n                    ^             ┏┓　　　┏┓                ^\n"); 
	printf("                    ^          ┏┛┻━━━┛┻┓               ^\n");
	printf("                    ^          ┃　　　　　　　┃               ^\n");
	printf("                    ^          ┃　　　━　　　┃               ^\n");
	printf("                    ^          ┃　┳┛　┗┳　┃               ^\n");
	printf("                    ^          ┃　　　　　　　┃               ^\n");
	printf("                    ^          ┃　　　┻　　　┃               ^\n");
	printf("                    ^          ┃　　　　　　　┃               ^\n");
	printf("                    ^          ┗━┓　　　┏━┛               ^\n");
	printf("                    ^          ┃　　　┃ 学生成绩              ^\n");
	printf("                    ^          ┃　　　┃ 管理系统              ^\n");
	printf("                    ^          ┃　　　┗━━━┓               ^\n");
	printf("                    ^          ┃　　　　　　　┣┓             ^\n");
	printf("                    ^          ┃　　　　　　　┏┛             ^\n");
	printf("                    ^          ┗┓┓┏━┳┓┏┛               ^\n");
	printf("                    ^          ┃┫┫　┃┫┫                   ^\n");
	printf("                    ^          ┗┻┛　┗┻┛                   ^\n\n");
	printf("                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	while (1)
	{
		printf("\n\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		printf("                    $请输入管理员账号：       $\n");
		printf("                     ");
		scanf("%d", &account);
		printf("                    $请输入密码：             $\n");
		printf("                     ");
		scanf("%d", &pass);
		printf("\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
		if (account == 111 && pass == 123)
		{
			break;
		}
		else
		{
			printf("                    输入信息有误请重新输入！\n");
		}
	}
	system("cls");
}

int input_menu()
{
	int n, m = 3;
	STUDENT stu[MAX];
	float rat[3];
	printf("\n\n                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	printf("                    $请输入学生人数(小于100)                      $\n                    ");
	scanf("%d", &n);
	printf("\n                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
	input(stu, n, m);
	input_ratios(rat);
	getAvgScore(stu, rat, n, m);
	write(stu, n, "input.data");
	return n;
}

int chose_menu()
{
	int opse;
	printf("                    ********************************************\n");
	printf("                    *                                          *\n");
	printf("                    *      1、打印按学号排序学生               *\n");
	printf("                    *      2、打印按数学成绩排序学生           *\n");
	printf("                    *      3、打印按英语成绩排序学生           *\n");
	printf("                    *      4、打印按计算机成绩排序学生         *\n");
	printf("                    *      5、打印按平均成绩排序学生           *\n");
	printf("                    *      6、打印按加权平均成绩排序学生       *\n");
	printf("                    *      7、打印各科成绩统计结果             *\n");
	printf("                    *      8、查询——按该生学号               *\n");
	printf("                    *      9、查询——按该生姓名               *\n");
	printf("                    *      0、退    出                         *\n");
	printf("                    *                                          *\n");
	printf("                    ********************************************\n");
	printf("\n\n                    请输你的选择：");
	scanf("%d", &opse);
	while (opse < 0 || opse > 9)
	{
		printf("\n                    输入有误，请重新输你的选择：\n                    ");
		scanf("%d", &opse);
	}
	return opse;
}

int chose_menu_opse()
{
	int opse;
	printf("\n\n                    ================================================\n");
	printf("                    |                                              |\n");
	printf("                    |      1-返回选择界面       2-添加学生         |\n");
	printf("                    |                  0-退    出                  |\n");
	printf("                    |                                              |\n");
	printf("                    ================================================\n");
	printf("\n\n                    请输你的选择：");
	scanf("%d", &opse);
	while (opse < 0 || opse > 2)
	{
		printf("\n                    输入有误，请重新输你的选择：\n                    ");
		scanf("%d", &opse);
	}
	return opse;
}

void main_menu()
{
	int n, m = 3;
	STUDENT stu[MAX];
	float rat[3];
	int opse;
	int flag[7] = { 0,0,0,0,0,0,0 };

	//首页
	first_menu();

	//输入页面
	n = input_menu();/*返回学生人数*/

	//n = 7;

	read(stu, n, "input.data");
	//output(stu, n);


	//选择页面
	opse = chose_menu();
	system("cls");

	do
	{	
		switch (opse)
		{
		case 1:
			if (flag[0] == 0)
			{
				printf("\n                    ---------------------------------按学号排序如下-----------------------------------------------\n");
				Quick_Sort(stu, 0, n - 1, 6);
				write(stu, n, "quick_sno.data");
				flag[0] = 1;
			}
			else
			{
				printf("\n                    ---------------------------------按学号排序如下-----------------------------------------------\n");
				read(stu, n, "quick_sno.data");
			}
			output(stu, n);//打印未排序学生
			break;

		case 2:
			if (flag[1] == 0)
			{
				printf("\n                    -------------------------------按数学成绩排序如下---------------------------------------------\n\n");
				Quick_Sort(stu, 0, n - 1, 1);//打印按数学成绩排序学生
				write(stu, n, "quick_math.data");
				flag[1] = 1;
			} 
			else
			{
				printf("\n                    -------------------------------按数学成绩排序如下---------------------------------------------\n\n");
				read(stu, n, "quick_math.data");
			}
			output(stu, n);
			break;

		case 3:
			if (flag[2] == 0)
			{
				printf("\n                    -------------------------------按英语成绩排序如下---------------------------------------------\n\n");
				Quick_Sort(stu, 0, n - 1, 2);//打印按英语成绩排序学生
				write(stu, n, "quick_english.data");
				flag[2] = 1;
			}
			else
			{
				printf("\n                    -------------------------------按英语成绩排序如下---------------------------------------------\n\n");
				read(stu, n, "quick_english.data");
			}
			output(stu, n);
			break;

		case 4:
			if (flag[3] == 0)
			{
				printf("\n                    ------------------------------按计算机成绩排序如下--------------------------------------------\n\n");
				Quick_Sort(stu, 0, n - 1, 3);//打印按计算机成绩排序学生 
				write(stu, n, "quick_cs.data");
				flag[3] = 1;
			}
			else
			{
				printf("\n                    ------------------------------按计算机成绩排序如下--------------------------------------------\n\n");
				read(stu, n, "quick_cs.data");
			}
			output(stu, n);
			break;

		case 5:
			if (flag[4] == 0)
			{
				printf("\n                    -------------------------------按平均成绩排序如下---------------------------------------------\n\n");
				Quick_Sort(stu, 0, n - 1, 4);//打印按平均成绩排序学生 
				write(stu, n, "quick_avg.data");
				flag[4] = 1;
			}
			else
			{
				printf("\n                    -------------------------------按平均成绩排序如下---------------------------------------------\n\n");
				read(stu, n, "quick_avg.data");
			}
			output(stu, n);
			break;

		case 6:
			if (flag[5] == 0)
			{
				printf("\n                    -----------------------------按加权平均成绩排序如下-------------------------------------------\n\n");
				Quick_Sort(stu, 0, n - 1, 5);//打印按平均成绩排序学生 
				write(stu, n, "quick_ravg.data");
				flag[5] = 1;
			}
			else
			{
				printf("\n                    -----------------------------按加权平均成绩排序如下-------------------------------------------\n\n");
				read(stu, n, "quick_ravg.data");
			}
			output(stu, n);
			break;

		case 7:
			if (flag[6] ==  0)
			{
				data_analyse(stu, n);//打印各科成绩统计结果
				flag[6] = 1;
			}
			else
			{
				read_all_result();
			}
			break;
		case 8:
			search_Sno(stu, n);
			break;
		case 9:
			search_Name(stu, n);
			break;
		}
		if (opse != 0)
		{
			opse = chose_menu_opse();
			if (opse == 1)
			{
				system("cls");
				opse = chose_menu();
			}
			else if(opse == 2)
			{
				system("cls");
				n = add_student(n);
				opse = chose_menu();
				read(stu, n, "input.data");
				for (int i = 0; i < 7; i++)
				{
					flag[i] = 0;
				}
			}
			system("cls");
		}
	} while (opse != 0 );
}
/*--------------------------------------------------------------------------------*/

int main()
{
	main_menu();
}