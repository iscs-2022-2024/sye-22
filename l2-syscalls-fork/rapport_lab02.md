# SYE22 - Laboratoire 2 - Syscalls et processes

Auteur : Rayane Annen

## Étape 2 

Nous devons ajouter un nouveau syscall appelé `sys_fork2` il possède les même fonctionnalités que `sys_fork` mais doit afficher un message supplémentaire provenant du kernel (voir consigne du labo)

`fork2(): process test_fork2.elf with pid 2 forked to child with pid 3`

On nous indique que les fichiers à modifier sont les suivant :
 - so3/include/syscall.h
 - so3/include/process.h
 - so3/kernel/process.c
 - so3/kernel/syscalls.c
 - usr/lib/libc/include/syscall.h
 - usr/lib/libc/crt0.S

### `so3/include/syscall.h`

Dans ce fichier on définit notre numéro d'interruption qui sera `112` en ajoutant la ligne suivante :

```c
#define SYSCALL_FORK2       112
```

### `so3/include/process.h`

Dans ce fichier ont duplique la définition de la fonction `do_fork` et définissons la fonction `do_fork2`.

```c
int do_fork2(void);
```

### `so3/include/process.c`

On duplique l'implémentation de la fonction `do_fork` pour `do_fork2` et on modifie celle-ci afin d'ajouter le message demandé dans la consigne qui indique simplement qu'un processus fork un enfant en précisant les Id de processus.

```c
    ...
	local_irq_restore(flags);
    
    // sys_fork2 logging
	printk("fork2(): process %s with pid %d forked to child with pid %d\n", parent->name, parent->pid, newp->pid);
    ...
    return newp->pid;

```

### `so3/kernel/syscalls.c`

Dans ce fichier nous allons indiquer l'existence d'un nouveau syscall et quelle fonction exécutée quand on doit l'appeler.

On rajoute alors un cas supplémentaire, après `SYSCALL_FORK`

Ce code est ajoutée dans la fonction qui gère les appels systèmes de signature :

```c
long syscall_handle(unsigned long r0, unsigned long r1, unsigned long r2, unsigned long r3)
```

```c
case SYSCALL_FORK2:
    result = do_fork2();
    break;
```

### `usr/lib/libc/include/syscall.h`

Dans ce fichier, nous étions censé ajouter la définition de la fonction `sys_fork2`, mais cela était déjà fait.

```c
int sys_fork2(void);
```

On définit cependant une macro supplémentaire pour le stub qu'on va implémenter.

```c
#define syscallFork2            112
```

### `usr/lib/libc/crt0.S`

On définit le stub dans ce fichier en utilisant l'instruction `SYSCALLSTUB`.

```arm
SYSCALLSTUB sys_fork2         syscallFork2
```