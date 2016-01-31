This is a different version of the crossword helper program you can find in my repositories.

This time, the dictionaries are read into struct arrays at the start of the program before we start looking for words.

Binary search is used to find the first match (for anagrams) or the first word of the correct length (for missing letter searches).

The word lists come from the 12dict project. I have merged the files "neol2007.txt", "2of4brif.txt" and "2of12inf.txt". There are 104420 entries.

The file "words_by_length.txt" consists of all the words sorted by length.
The file "anag_sorted.txt" contains all the words with their letters sorted first, followed by the actual word. These are sorted alphabetically.

If you type 't' instead of 'q' for quit or entering a query, the program prints out every word in the dictionary that has an anagram. The output can be found in "all.txt".
