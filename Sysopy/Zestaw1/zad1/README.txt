W bibliotece przyjąłem, że ważniejszą dla użytkownika sprawą będzie 
Wyszukanie wyszukanie elementu o podobnej długości niż dodawanie bądź usuwanie 
bloków (częściej będzie elementów wyszukiwał niż dodawał bądź je zmieniał).
Tak więc przy każdym dodaniu bloku sprawdzam sumę znaków dodanego bloku 
I przechowuje je w pamięci.

Dostępne funkcje w bibliotece to:
Charray * create_char_array(Charray * array, int array_length, int is_static);
Charray * resize_char_array(Charray * array, int new_array_length, int is_static);
Charray * delete_char_array(Charray * array, int is_static);
int sum_char_block(char * char_block);
Charray * create_char_block(Charray * array, int index, char * block_content, int is_static);
Charray * delete_char_block(Charray * array, int index, int is_static);
Charray * change_char_block(Charray * array, int index, char * new_block_content, int is_static);
int closest_bsum_index(Charray * array, int value);
void printCharray(Charray * array);