# Laboratoire 5 : ordonnancement et gestion mémoire

Auteur: Rayane Annen
Année: 2022-2023

## Etape 2 (renice())

a)
Complétion du code de `renice()`

```c
int do_renice(uint32_t pid, uint32_t new_prio) {

	if (new_prio == 0) {
		printk("do_renice(): priority must be greater than 0\n");
		return -1;
	}

	/* find process by pid */

	pcb_t *proc = find_proc_by_pid(pid);
	if (proc > 0) {
		/* change priority of the main thread */
		proc->main_thread->prio = new_prio;
	} else {
		printk("do_renice(): process %d not found\n", pid);
	}
	return 0;
}
```

On modifie uniquement la priorité du thread principal, puisqu'elle sera héritée dans les threads enfants

b)
 
On peut uniquement exécuter une commande avant que le processus `time_loop` ne prenne le dessus et bloque entièrement le shell. On voit les secondes défiler dans le shell, on ne peut plus accéder au shell nous même. Cela veut dire que le shell est aussi prioritaire que le processus `time_loop`.

c) 

En mode continue, sans changer de priorité, `time_loop` passe devant le shell et bloque totalement son utilisation après qu'on ait lancé une commande. Si on change la priorité du shell (commande: `renice 1 1`) avant de lancer `time_loop` pour qu'il soit plus prioritaire que le shell, `time_loop` prend le dessus dès son lancement.

d)

En mode bloquant, sans changer de priorité, le programme passe en background, il repasse dans l'état running juste après l'exécution d'une commande, et passe en waiting aussitôt pour laisser le programme que l'on souhaite exécuter passer à l'état running. 

Si on change la priorité du shell, le programme fait des allers-retour entre l'état waiting, ready et running, ainsi on peut à la fois utiliser le shell et voir le compteur s'incrémenter. Cela est possible car le processus `time_loop` fait un appel système à `usleep`, ce qui met le processus en attente et permet de laisser un accès au shell.

## Étape 3

a) Le fichier est `so3/mm/heap.c`
b) quick-fit (l.108 dans `heap.c`)
c) `so3/arch/arm32/mmu.c`

f) 

Exemple de sortie : 

```
so3% memory
page 4KiB: virt: 0x20c018, phy: 0x905cf018
so3% memory
page 4KiB: virt: 0x20c018, phy: 0x949e8018
```


L'adresse virtuelle ne change pas mais l'adresse physique oui, cela est dû à l'algorithme quick-fit, qui, en ce qui concerne l'adresse physique va essayer de trouver le meilleur emplacement mémoire à chaque allocation. 

L'adresse virtuelle ne change pas car l'espace d'adressage virtuelle est isolé pour chaque programme.

Les adresses physiques se terminent toujours par `0x018`, car c'est par cette séquence que commence l'espace d'adressage disponible à l'utilisateur. 

On voit aussi que les adresses physiques sont bien séparée par 0x1000, représentant leur taille.