#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;
struct pwd_names;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

typedef struct pwd_names {
	char *name;
	struct pwd_names* next;
} pwd_names;

void touch (Dir* parent, char* name) {
	File *file, *head;

	// creez fisier
	file = malloc(sizeof(File));
	file->name = malloc(strlen(name) + 1);
	strcpy(file->name, name);
	file->parent = parent;
	file->next = NULL;

	// nu exista fisiere
	if (!parent->head_children_files) {
		parent->head_children_files = file;
		return;
	}

	// verifica daca exista fisier cu acelasi nume
	// sau adauga la final fisierul creat
	head = parent->head_children_files;
	while (head->next) {
		if (!strcmp(head->name, name)) {
			printf("File already exists\n");
			free(file->name);
			free(file);
			return;
		}
		head = head->next;
	}

	if (!strcmp(head->name, name)) {
		printf("File already exists\n");
		free(file->name);
		free(file);
		return;
	}

	head->next = file;
}

void mkdir (Dir* parent, char* name) {
	Dir *dir, *head;

	// creeaza director
	dir = malloc(sizeof(Dir));
	dir->name = malloc(strlen(name) + 1);
	strcpy(dir->name, name);
	dir->parent = parent;
	dir->head_children_files = NULL;
	dir->head_children_dirs = NULL;
	dir->next = NULL;

	// verifica daca exista directoare
	if (!parent->head_children_dirs) {
		parent->head_children_dirs = dir;
		return;
	}

	// verifica daca exista director cu acelasi nume
	// sau adauga la final directorul creat
	head = parent->head_children_dirs;
	while (head->next) {
		if (!strcmp(head->name, name)) {
			printf("Directory already exists\n");
			free(dir->name);
			free(dir);
			return;
		}
		head = head->next;
	}

	if (!strcmp(head->name, name)) {
		printf("Directory already exists\n");
		free(dir->name);
		free(dir);
		return;
	}

	head->next = dir;
}

void ls (Dir* parent) {
	File *head_file;
	Dir *head_dir;

	head_file = parent->head_children_files;
	head_dir = parent->head_children_dirs;

	// afiseaza directoarele
	while (head_dir) {
		printf("%s\n", head_dir->name);
		head_dir = head_dir->next;
	}

	// afiseaza fisierele
	while (head_file) {
		printf("%s\n", head_file->name);
		head_file = head_file->next;
	}
}

void rm (Dir* parent, char* name) {
	File *del, *head;
	int ok = 0;

	head = parent->head_children_files;

	// nu exista fisiere
	if (!head) {
		printf ("Could not find the file\n");
		return;
	}

	// fisierul cautat e primul in lista
	if (!strcmp(head->name, name)) {
		parent->head_children_files = head->next;
		free(head->name);
		free(head);
		return;
	}

	// cauta fisierul
	while (head->next) {
		if (!strcmp(head->next->name, name)) {
			ok = 1;
			break;
		}
		head = head->next;
	}

	// fisierul nu e gasit sau e sters
	if (ok == 0) {
		printf ("Could not find the file\n");
	} else {
		del = head->next;
		head->next = head->next->next;
		free(del->name);
		free(del);
	}
}

void rmdir_recursive(Dir* target) {
	if (!target) {
		return;
	}

	Dir *dir, *next_dir;
	File *file, *next_file;

	dir = target->head_children_dirs;
	file = target->head_children_files;

	// se sterg recursiv directoarele
	while (dir) {
		next_dir = dir->next;
		rmdir_recursive(dir);
		free(dir->name);
		free(dir);
		dir = next_dir;
	}

	// se sterg fisierele din directorul curent
	while (file) {
		next_file = file->next;
		free(file->name);
		free(file);
		file = next_file;
	}
}

void rmdir (Dir* parent, char* name) {
	Dir *dir, *curr;
	int ok = 0;

	curr = parent->head_children_dirs;

	// nu exista directoare
	if (!curr) {
		printf("Could not find the dir\n");
		return;
	}

	// directorul cautat este primul
	if (!strcmp(curr->name, name)) {
		parent->head_children_dirs = curr->next;
		rmdir_recursive(curr);
		free(curr->name);
		free(curr);
		return;
	}

	// se cauta directorul
	while (curr->next) {
		if (!strcmp(curr->next->name, name)) {
			ok = 1;
			break;
		}
		curr = curr->next;
	}

	// nu exista directorul cautat
	if (!ok) {
		printf("Could not find the dir\n");
		return;
	}

	dir = curr->next;
	curr->next = curr->next->next;

	// se va sterge directorul
	rmdir_recursive(dir);
	free(dir->name);
	free(dir);
}

void cd(Dir** target, char *name) {
	// argument ".."
	if (!strcmp(name, "..")) {
		if (!strcmp((*target)->name, "home")) {
			return;
		} else {
			(*target) = (*target)->parent;
			return;
		}
	}

	// nu exista directoare
	if (!(*target)->head_children_dirs) {
		printf ("No directories found!\n");
		return;
	}

	Dir *dir;

	// se cauta si se acceseaza directorul destinatie
	dir = (*target)->head_children_dirs;
	while (dir) {
		if (!strcmp(dir->name, name)) {
			(*target) = dir;
			return;
		}
		dir = dir->next;
	}

	// nu exista directorul
	if (!dir) {
		printf ("No directories found!\n");
	}
}

char *pwd (Dir* target) {
	int length = 0;
	pwd_names *new, *head, *curr, *next;

	length = length + strlen(target->name) + 1;

	// lista inlantuita care retine numele elementelor
	// din path
	new = malloc(sizeof(pwd_names));
	new->name = malloc(strlen(target->name) + 1);
	strcpy(new->name, target->name);
	new->next = NULL;
	head = new;

	while (strcmp(target->name, "home")) {
		target = target->parent;
		new = malloc(sizeof(pwd_names));
		new->name = malloc(strlen(target->name) + 1);
		strcpy(new->name, target->name);
		new->next = head;
		head = new;
		length = length + strlen(target->name) + 1;
	}
	length++;

	char *pwd_path = malloc(length * sizeof(char));

	// creez numele path-ului folosindu-ma de lista creata
	pwd_path[0] = '\0';
	curr = head;
	while (curr) {
		strcat(pwd_path, "/");
		strcat(pwd_path, curr->name);
		curr = curr->next;
	}

	while (head) {
		next = head->next;
		free(head->name);
		free(head);
		head = next;
	}

	return pwd_path;
}

void stop (Dir* target) {
	Dir *dir, *next_dir;
	File *file, *next_file;

	// ajunge in home
	while (strcmp(target->name, "home")) {
		target = target->parent;
	}

	// sterge fisierele
	file = target->head_children_files;
	while(file) {
		next_file = file->next;
		free(file->name);
		free(file);
		file = next_file;
	}

	// sterge recursiv directoarele
	dir = target->head_children_dirs;
	while (dir) {
		next_dir = dir->next;
		rmdir_recursive(dir);
		free(dir->name);
		free(dir);
		dir = next_dir;
	}

	free(target->name);
	free(target);
}

void tree (Dir* target, int level) {
	if (!target) {
		return;
	}

	Dir *dir;
	File *file;

	dir = target->head_children_dirs;
	file = target->head_children_files;

	// DFS recursiv pe directoare + afisare
	while (dir) {
		for (int i = 0; i < level; ++i) {
			printf("    ");
		}
		printf("%s\n", dir->name);
		tree(dir, level + 1);

		dir = dir->next;
	}

	// afisare fisiere
	while (file) {
		for (int i = 0; i < level; ++i) {
			printf("    ");
		}
		printf("%s\n", file->name);
		file = file->next;
	}
}

void mv(Dir* parent, char *oldname, char *newname) {
	int ok_file = 1, ok_dir = 1, ok_file_name = 1, ok_dir_name = 1;
	File *file, *curr_file, *prev_file;
	Dir *dir, *curr_dir, *prev_dir;

	file = parent->head_children_files;
	dir = parent->head_children_dirs;

	// verific daca exista fisierul cautat
	while (file) {
		if (!strcmp(file->name, oldname)) {
			break;
		}
		file = file->next;
	}

	if (!file) {
		ok_file = 0;
	}
	
	// verific daca exista directorul cautat
	while (dir) {
		if (!strcmp(dir->name, oldname)) {
			break;
		}
		dir = dir->next;
	}

	if (!dir) {
		ok_dir = 0;
	}

	// nu exista nici fisier nici director
	// cu numele cautat
	if (!ok_dir && !ok_file) {
		printf("File/Director not found\n");
		return;
	}
	
	// verific daca exista fisier cu numele nou
	curr_file = parent->head_children_files;
	while (curr_file) {
		if (!strcmp(curr_file->name, newname)) {
			ok_file_name = 0;
			break;
		}
		curr_file = curr_file->next;
	}

	// verific daca exista director cu numele nou
	curr_dir = parent->head_children_dirs;
	while (curr_dir) {
		if (!strcmp(curr_dir->name, newname)) {
			ok_dir_name = 0;
			break;
		}
		curr_dir = curr_dir->next;
	}

	// exista deja fisier/director cu numele nou
	if (!ok_file_name || !ok_dir_name) {
		printf("File/Director already exists\n");
		return;
	}

	if (ok_file) {
		// se poate inlocui numele fisierului cu numele dat
		if (ok_file_name && ok_dir_name) {
			strcpy(file->name, newname);

			prev_file = parent->head_children_files;
			
			// fisierul de inlocuit e primul
			if (prev_file == file) {
				// e un singur fisier
				if (!prev_file->next) {
					return;
				}

				// sunt mai multe fisiere
				parent->head_children_files = prev_file->next;
				file->next = NULL;
				prev_file = parent->head_children_files;
				while (prev_file->next) {
					prev_file = prev_file->next;
				}
				prev_file->next = file;
				return;
			}

			// fisierul de inlocuit nu e primul
			while (prev_file->next != file) {
				prev_file = prev_file->next;
			}
			prev_file->next = prev_file->next->next;
			file->next = NULL;
			
			prev_file = parent->head_children_files;
			while (prev_file->next) {
				prev_file = prev_file->next;
			}
			prev_file->next = file;
			return;
		}
	}

	if (ok_dir) {
		// se poate inlocui numele directorului cu numele dat
		if (ok_file_name && ok_dir_name) {
			strcpy(dir->name, newname);

			prev_dir = parent->head_children_dirs;
			
			// directorul de inlocuit e primul
			if (prev_dir == dir) {
				// e un singur director
				if (!prev_dir->next) {
					return;
				}

				// sunt mai multe directoare
				parent->head_children_dirs = prev_dir->next;
				dir->next = NULL;
				prev_dir = parent->head_children_dirs;
				while (prev_dir->next) {
					prev_dir = prev_dir->next;
				}
				prev_dir->next = dir;
				return;
			}

			// directorul de inlocuit nu e primul
			while (prev_dir->next != dir) {
				prev_dir = prev_dir->next;
			}
			prev_dir->next = prev_dir->next->next;
			dir->next = NULL;
			
			prev_dir = parent->head_children_dirs;
			while (prev_dir->next) {
				prev_dir = prev_dir->next;
			}
			prev_dir->next = dir;
			return;
		}
	}

}

// identifica comanda
int choose_command(char *cmd_line) {
	char *argum = strtok(cmd_line, " \n");

	if (!strcmp(argum, "touch")) {
		return 1;
	} else if (!strcmp(argum, "mkdir")) {
		return 2;
	} else if (!strcmp(argum, "ls")) {
		return 3;
	} else if (!strcmp(argum, "rm")) {
		return 4;
	} else if (!strcmp(argum, "rmdir")) {
		return 5;
	} else if (!strcmp(argum, "cd")) {
		return 6;
	} else if (!strcmp(argum, "tree")) {
		return 7;
	} else if (!strcmp(argum, "pwd")) {
		return 8;
	} else if (!strcmp(argum, "mv")) {
		return 9;
	}

	return -1;
}

int main () {
	char *cmd_line = malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	char *argum, *pwd_path, *oldname, *newname;
	int command;
	Dir *parent;

	// initializarea directorului home
	parent = malloc(sizeof(Dir));
	parent->name = malloc(sizeof("home"));
	strcpy(parent->name, "home");
	parent->parent = NULL;
	parent->head_children_files = NULL;
	parent->head_children_dirs = NULL;
	parent->next = NULL;

	do
	{
		fgets(cmd_line, MAX_INPUT_LINE_SIZE, stdin);
		if (!strcmp(cmd_line, "stop\n")) {
			continue;
		}

		command = choose_command(cmd_line);

		// apeleaza comanda specifica
		switch (command) {
			case 1:
					touch(parent, strtok(NULL, " \n"));
					break;
			case 2:
					mkdir(parent, strtok(NULL, " \n"));
					break;
			case 3:
					ls(parent);
					break;
			case 4:
					rm(parent, strtok(NULL, " \n"));
					break;
			case 5:
					rmdir(parent, strtok(NULL, " \n"));
					break;
			case 6:
					cd(&parent, strtok(NULL, " \n"));
					break;
			case 7:
					tree(parent, 0);
					break;
			case 8:
					pwd_path = pwd(parent);
					printf("%s\n", pwd_path);
					free(pwd_path);
					break;
			case 9:
					oldname = strtok(NULL, " \n");
					newname = strtok(NULL, " \n");
					mv(parent, oldname, newname);
					break;
		}
	} while (strcmp(cmd_line, "stop\n"));

	stop(parent);

	free(cmd_line);
	
	return 0;
}
