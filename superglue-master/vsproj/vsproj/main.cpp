
int main_logging(int argc, char *argv[]);
int main_dep();
int main_nbody(int argc, char *argv[]);
int main_nested1();
int main_subtasks();
int main_subtasksdeps();
int main_superglue(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	main_logging(argc, argv);
	main_dep();
//	main_nbody(argc, argv);	// fail to compile
//	main_nested1();	// inf loops??
//	main_subtasks();	//OK
//	main_subtasksdeps();	// crashes
	main_superglue(argc, argv);
}