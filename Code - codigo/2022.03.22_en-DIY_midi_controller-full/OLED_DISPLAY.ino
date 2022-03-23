#ifdef USING_DISPLAY

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_BANKS_WITH_BUTTONS

/////////////////////////////////////////////
// prints the MIDI channel in the display
void printChannel(int n) {
  // display.setCursor(display_pos_x, display_pos_y);  // posição onde vai começar a ser mostrado na tela (x,y)
  display.clearDisplay();  // Clear the display so we can refresh

  // Print text:
  display.setCursor(0, 0);  // posição onde vai começar a ser mostrado na tela (x,y)
  display.setTextSize(1);
  display.print("ch");  // Imprime no display

  // displays the number
  display.setTextSize(display_text_size);
  display.setCursor(display_pos_x, display_pos_y);  // posição onde vai começar a ser mostrado na tela (x,y)

  if (n < 9) {
    display.print("0"); // insere um zero se for menor que 10
    //display.display();
  }

#ifdef ATMEGA328
  display.print(n);  // Imprime no display
  display.display();
#elif ATMEGA32U4
  display.print(n + 1); // Imprime no display
  display.display();
#endif

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif // USING_BANKS_WITH_BUTTONS


#ifdef USING_OCTAVE

void printOctave(int n) {


  // Print text:
  // Print text:


  // displays the number
  display.clearDisplay();  // Clear the display so we can refresh
  display.setCursor(0, 0);  // posição onde vai começar a ser mostrado na tela (x,y)
  display.setTextSize(1);
  display.print("Oct:");  // Imprime no display
  display.setTextSize(display_text_size - 2);
  display.setCursor(display_pos_x, display_pos_y);  // posição onde vai começar a ser mostrado na tela (x,y)


  display.print(n);  // Imprime no display
  display.display();

}

#endif // USING_OCTAVE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif // USING_DISPLAY
