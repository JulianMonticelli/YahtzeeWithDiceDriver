#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STRING_LENGTH 21
// OLD DICE macro
//#define DICE test_get_random_dice
// NEW DICE macro
#define DICE() fread(dice_roll, 1, 5, dice_driver)
// function prototypes
int test_get_random_dice(); // used for testing yahtzee program

int total_score();
int score(int n);
int score_3_of_kind();
int score_4_of_kind();
int score_full_house();
int score_small_straight();
int score_large_straight();
int score_yahtzee();
int score_chance();
int compare_int(const void * a, const void * b);

void null_error();
void print_dice();
void print_categories();
void flush_stdin();

void upper_menu();
void lower_menu();

//begin listing variables nyah
int bonus_upper_sec;
int * dice;
int * scores;
char categories[13][STRING_LENGTH]
	= { "Ones", 			// 0
		"Twos", 			// 1
		"Threes", 			// 2
		"Fours",			// 3
		"Fives",			// 4
		"Sixes",			// 5
		"Three of a Kind",	// 6
		"Four of a Kind",	// 7
		"Full House",		// 8
		"Small straight",	// 9
		"Large straight",	// 10
		"Yahtzee",			// 11
		"Chance" };			// 12
		
int main() {
	FILE * dice_driver;
	dice_driver = fopen("/dev/dice", "r"); // this should open the file
	//srand(time(NULL));// REMOVE THIS AFTER YOU IMPLEMENT DICE FUNCTION
	bonus_upper_sec = 0; // init bonus to 0
	dice = (int *)malloc(5 * sizeof(int)); // init dice array
	
	int i = 0; // declare & initialize iterator
	
	char dice_roll[5];
	DICE();
	for(i = 0; i < 5; i++) {
		dice[i] = (int)dice_roll[i];
	}
	scores = (int *)malloc(13 * sizeof(int)); // init scores array
	for(i = 0; i < 13; i++) {
		scores[i] = -1; // to prevent confusion of unused rule - sentinel value because 0 is legitimate score
	}
	int num_scored  = 0; // init number of scored categories
	
	printf("Your roll:\n"); // first roll
	
	while(num_scored < 13) { // game loop
		int reroll[5]; // reroll dice array 
		for(i = 0; i < 5; i++)
			reroll[i] = -1; // to be sentenalized
		
		int num_rerolls = 0; // max 2 rerolls
		
		while(num_rerolls < 2 && reroll[0] != 0) {
			if(num_rerolls == 0) print_dice();
			printf("Which dice to reroll?  "); // Q.
			char reroll_string[STRING_LENGTH]; // sentinel value
			fgets(reroll_string, STRING_LENGTH, stdin); // A.
			i = 0; // reusing iterator
			char c = reroll_string[i]; // to start while, we need initial value
			if(c == NULL) null_error();
			while(c != NULL) {
				// Index-delimiting, not space delimiting
				if(i % 2 == 1) { // this should be the delimeter - works with any single char delimeter
					c = reroll_string[++i]; // next char in reroll string
					continue; // next iteration pls
				}
				// Is 0?
				if(c - '0' == 0) { // 0 detection anywhere on even-index char results in not rerolling
					reroll[0] = 0; // !!!---!!! WARNING: DO NOT ENTER A 0 VAL UNLESS YOU WANT TO NOT ROLL
					break; // 0 detected - shouldn't worry about anything else
				} else { // should be a number at this point
					reroll[ (i/2) ] = (int)(c - '0');
				}
				c = reroll_string[++i]; // next char in reroll string
			}
			if(reroll[0] == 0) // need to break out if reroll[0] is 0
				break;
			else DICE(); // if we're still proceeding, roll 5 new dice
			for(i = 0; i < 5; i++) {
				if(reroll[i] != -1) { // if we don't reroll
					dice[reroll[i]-1] = dice_roll[reroll[i]-1]; // dice of reroll is equal to rerolled dice we are rerolling (confusing wording, I know)
					reroll[i] = -1; // resentinalize after reroll :)
				}
			}
			num_rerolls++;
			print_dice();
		}
		// should be where we begin asking what category you want to use
		int category = 0;
		int choice = 0;
		char needs_valid_choice = 1; // control variable
		while(needs_valid_choice) {
			printf("\nWhat section would you like to add this roll?\n1.) Upper Section\n2.) Lower Section\n\nSelection? "); // Q.
			scanf("%d", &category); // A.
			flush_stdin();
			if(category > 1) lower_menu(); // 2... and anything above 2 results in lower menu (i.e., typos)
			else upper_menu(); // 1? 0? -1? give them the upper menu
			printf("\nSelection? ");
			scanf("%d", &choice);
			flush_stdin();
			if(category > 1) { // lower menu
				choice += 5; // selection 1 would actually be index 6
				if(choice == 6 && scores[choice] == -1) { // 3 of kind 
					scores[choice] = score_3_of_kind();
					needs_valid_choice = 0;
				} else if (choice == 7 && scores[choice] == -1) { // 4 of a kind 
					scores[choice] = score_4_of_kind();
					needs_valid_choice = 0;
				} else if (choice == 8 && scores[choice] == -1) { // full house
					scores[choice] = score_full_house();
					needs_valid_choice = 0;
				} else if (choice == 9 && scores[choice] == -1) { // small straight
					scores[choice] = score_small_straight();
					needs_valid_choice = 0;
				} else if (choice == 10 && scores[choice] == -1) { // large straight
					scores[choice] = score_large_straight();
					needs_valid_choice = 0;
				} else if (choice == 11 && scores[choice] == -1) {
					scores[choice] = score_yahtzee();
					needs_valid_choice = 0;
				} else if (choice == 12 && scores[choice] == -1) {
					scores[choice] = score_chance();
					needs_valid_choice = 0;
				} else if (choice > 12 || choice < 6) {
					printf("\nChoice range validation failed! >7 || <1!\n");
				} else {
					printf("\nThat choice has already been taken!\n");
				}
			} else { // upper menu
				if(scores[choice-1] == -1) {
					scores[choice-1] = score(choice); // update score
					int running_total_upper_sec = 0;
					for(i = 0; i < 6; i++) {
						running_total_upper_sec += scores[i];
						if(running_total_upper_sec >= 63) {
							bonus_upper_sec = 35;
						}
					}
					needs_valid_choice = 0;
				} else {
					printf("\nThat choice has already been taken!\n");
				}
			}
		}
		int current_score = total_score();
		printf("\n\nCurrent score:\t%d\n\n", current_score);
		num_scored++; // increment scored catagories
		print_categories();
	} // end of game loop
	
	int final_score = total_score();
	printf("Game over! Your score:\t%d\n\n", final_score);
	print_categories();
	return 0;
}

int test_get_random_dice() {
	int dice_roll = rand();
	dice_roll = (dice_roll % 6) + 1;
	return dice_roll;
}

void print_dice() {
	printf("\n%d %d %d %d %d\n\n", dice[0], dice[1], dice[2], dice[3], dice [4]);
	return;
}

void print_categories() {
	//  --  UPPERS:
	printf("%s: ",categories[0]);
	if(scores[0] != -1) printf("%d",scores[0]);
	printf("\t\t\t\t%s: ", categories[3]);
	if(scores[3] != -1) printf("%d",scores[3]);
	printf("\n"); // END LINE Ones & Fours
	
	printf("%s: ",categories[1]);
	if(scores[1] != -1) printf("%d",scores[1]);
	printf("\t\t\t\t%s: ", categories[4]);
	if(scores[4] != -1) printf("%d",scores[4]);
	printf("\n"); // END LINE Twos & Fives
	
	printf("%s: ",categories[2]);
	if(scores[2] != -1) printf("%d",scores[2]);
	printf("\t\t\t%s: ", categories[5]);
	if(scores[5] != -1) printf("%d",scores[5]);
	printf("\n"); // END LINE Threes & Sixes
	
	printf("Bonus Upper Section: %d\n", bonus_upper_sec); // Upper Section Bonus
	
	//  --  LOWERS:
	
	printf("%s: ",categories[6]);
	if(scores[6] != -1) printf("%d",scores[6]);
	printf("\t\t%s: ", categories[7]);
	if(scores[7] != -1) printf("%d",scores[7]);
	printf("\n"); // END LINE 3 & 4 Kind
	
	printf("%s: ",categories[9]);
	if(scores[9] != -1) printf("%d",scores[9]);
	printf("\t\t%s: ", categories[10]);
	if(scores[10] != -1) printf("%d",scores[10]);
	printf("\n"); // END LINE Small & Large straight
	
	printf("%s: ",categories[8]);
	if(scores[8] != -1) printf("%d",scores[8]);
	printf("\t\t\t%s: ", categories[11]);
	if(scores[11] != -1) printf("%d",scores[11]);
	printf("\n"); // END LINE Full house & Yahtzee
	
	printf("%s: ", categories[12]); // Chance
	if(scores[12] != -1) printf("%d\n\n",scores[12]);
	else printf("\n\n");
	return;
}

void null_error() {
	printf("Unexpected null value. Did you enter an empty string? Exiting program...");
	exit(0);
}

void lower_menu() {
	int i = 0;
	for(i = 0; i < 7; i++) {
		printf("%d.) %s\n", i+1, categories[i+6]);
	}
	printf("\n");
	return;
}

void upper_menu() {
	int i = 0;
	for(i = 0; i < 6; i++) {
		printf("%d.) %s\n", i+1, categories[i]);
	}
	printf("\n");
	return;
}

int total_score() {
	int i = 0;
	int total_score = 0;
	for(i = 0; i < 13; i++) {
		if(scores[i] != -1) total_score += scores[i]; // add each score to final score
	}
	total_score += bonus_upper_sec;
	return total_score;
}

int score(int n) {
	int i = 0;
	int total = 0;
	for(i = 0; i < 5; i++) {
		if(dice[i] == n)
			total += n;
	}
	return total;
}

int score_3_of_kind() {
	int copy[5];
	int i = 0;
	for(i = 0; i < 5; i++) { // boolean logic - bruteforce!
		copy[i] = dice[i]; // copy array
	}
	qsort(copy, 5, sizeof(int), compare_int); // qsort
	
	for(i = 2; i < 5; i++) {// start at index 2, and do this until index 4
		int prevprev = copy[i-2];
		int prev = copy[i-1];
		int curr = copy[i];
		if( (prevprev == prev) && (prev == curr) ) {
			int total = 0;
			for(i = 0; i < 5; i++) { // if we are here, we can overwrite i without consequence
				total += copy[i]; // b/c it just totals all of them
			}
			return total; // and returns total, exiting function
		}
	}
	return 0;	
}

int score_4_of_kind() {
	int copy[5];
	int i = 0;
	for(i = 0; i < 5; i++) {
		copy[i] = dice[i]; // copy array
	}
	qsort(copy, 5, sizeof(int), compare_int); // qsort
	
	for(i = 3; i < 5; i++) {// start at index 2, and do this until index 4
		int prevprevprev = copy[i-3];
		int prevprev = copy[i-2];
		int prev = copy[i-1];
		int curr = copy[i];
		if( (prevprevprev == prevprev) && (prevprev == prev) && (prev = curr) ) { // boolean logic - bruteforce!
			int total = 0;
			for(i = 0; i < 5; i++) { // if we are here, we can overwrite i without consequence
				total += copy[i]; // b/c it just totals all of them
			}
			return total; // and returns total, exiting function
		}
	}
	return 0;	
}

int score_full_house() {
	int copy[5];
	int i = 0;
	for(i = 0; i < 5; i++) {
		copy[i] = dice[i]; // copy array
	}
	qsort(copy, 5, sizeof(int), compare_int); // qsort
	
	if( ((copy[0] == copy[1]) && (copy[1] == copy[2])) && (copy[3] == copy[4]) ) // 3 & 2
		return 25;
	else if ((copy[2] == copy[3]) && (copy[3] == copy[4]) && (copy[0] == copy[1])) // 2 & 3
		return 25;
	else return 0; // no full house :(
}

int score_small_straight() {
	int copy[5];
	int i = 0;
	for(i = 0; i < 5; i++) {
		copy[i] = dice[i]; // copy array
	}
	qsort(copy, 5, sizeof(int), compare_int); // qsort
	
	if( (copy[0]+1 == copy[1]) && (copy[1]+1 == copy[2]) && (copy[2]+1 == copy[3]) ) // 4 sequential + 1
		return 30;
	else if ( (copy[1]+1 == copy[2]) && (copy[2]+1 == copy[3]) && (copy[3]+1 == copy[4]) ) // 1 + 4 sequential
		return 30;
	else return 0; // no small straight :(
}

int score_large_straight() {
	int copy[5];
	int i = 0;
	for(i = 0; i < 5; i++) {
		copy[i] = dice[i]; // copy array
	}
	qsort(copy, 5, sizeof(int), compare_int); // qsort
	
	if( (copy[0]+1 == copy[1]) && (copy[1]+1 == copy[2]) && (copy[2]+1 == copy[3]) && (copy[3]+1 == copy[4]) ) // 5 sequential
		return 40;
	else return 0; // no large straight :(
}

int score_yahtzee() {
	if( (dice[0] == dice[1]) && (dice[1] == dice[2]) && (dice[2] == dice[3]) && (dice[3] == dice[4]) ) // 5 equal
		return 50;
	else return 0; // no yahtzee :(
}

int score_chance() {
	int chance_total = 0;
	int i = 0;
	for(i = 0; i < 5; i++) {
		chance_total += dice[i];
	}
	return chance_total;
}

int compare_int(const void * a, const void * b)
{
  if ( *(int *)a <  *(int *)b ) return -1;
  if ( *(int *)a == *(int *)b ) return 0;
  if ( *(int *)a >  *(int *)b ) return 1;
}

void flush_stdin() // clears buffer
{
    int c; // integer
    do {
        c = getchar(); // use up the next char in the buffer
    } while (c != '\n' && c != EOF); // c == \n or EOF? we cleared the buffer
}