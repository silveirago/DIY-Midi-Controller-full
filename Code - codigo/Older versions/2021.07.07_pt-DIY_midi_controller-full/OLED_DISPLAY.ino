#ifdef USING_DISPLAY

/////////////////////////////////////////////
// prints the MIDI channel in the display
void printChannel(int n) {
  display.setCursor(display_pos_x, display_pos_y);  // posição onde vai começar a ser mostrado na tela (x,y)
  display.clearDisplay();  // Clear the display so we can refresh

  if (n < 10) {
    display.print("0"); // insere um zero se for menor que 10
    //display.display();
  }

  // Print text:

#ifdef ATMEGA328
  display.print(n);  // Imprime no display
  display.display();
#elif ATMEGA32U4
  display.print(n + 1); // Imprime no display
  display.display();
#endif
}

#endif
