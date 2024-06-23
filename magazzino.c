Magazzino espandi_magazzino(int ingId){
  int len = ingId + 1;

  magazzino->sez = realloc(magazzino->sez, sizeof(Ptr_lotto) * len);
  if(magazzino->sez == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }

  return magazzino;
}

Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt){

  if(testaLt == NULL) return lt;
  
  Ptr_lotto prec = NULL;
  for(Ptr_lotto temp = testaLt; temp != NULL; temp = temp->next){

    if(lt->scadenza < temp->scadenza){
      lt->next = temp;
      if(prec != NULL){
        prec->next = lt;
        return testaLt;
      }
      else{
        return lt;
      }
    }

    prec = temp;
  }

  prec->next = lt;

  return testaLt;
}

void aggiungi_lotto(Ptr_lotto lt){
  
  // Verifico se esiste una sezione del Magazzino per l'ingrediente, se no lo espando
  if(lt->ingId > magazzino.len){
    magazzino = espandi_magazzino(lt->ingId);
  }

  // Testa della linked list contenente tutti i lotti 
  Ptr_lotto lotti = magazzino.sez[ingId];

  lotti = inserisci_per_scadenza(lt, lotti);  
}

void dealloca_lotti(Ptr_lotto testaLt){

  Ptr_lotto prec = NULL;
  for(Ptr_lotto temp = testaLt; temp != NULL; temp = temp->next){
    if(prec != NULL){
      free(prec);
    }

    prec = temp;
  }

  if(prec != NULL) free(prec);

}

void dealloca_magazzino(){

  // Dealloca ogni singola serie di lotti
  for(size_t i = 0; i < magazzino.len; ++i){
    dealloca_lotti(magazzino.sez[i])
  }
  // Dealloca le sezioni del magazzino
  free(magazzino.sez);
}
