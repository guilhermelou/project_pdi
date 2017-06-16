	
	public double getMediaGlobal(){
		int numPixels=altura*largura;
		double mediaGlobal, soma = 0;		
		
		for(int row=0;row<altura;row++){
			for(int col=0;col<largura;col++){
				soma+=Matriz[row][col];
			}
		}
		mediaGlobal = soma/(double)numPixels;
		
		return mediaGlobal;
	}
	
	public double getVariancia(double mediaGlobal){
		int numPixels=altura*largura;
		double variancia, soma=0;
		
		for(int row=0;row<altura;row++){
			for(int col=0;col<largura;col++){
				soma+=Math.pow(((double)Matriz[row][col]-mediaGlobal), 2);
			}
		}
		variancia = soma/(double)numPixels;
		
		return variancia;
	}
	
	public void equalizarEstatistica(){		
		double mediaGlobal, desvioPadraoGlobal;				
		double mediaLocal, desvioPadraoLocal;
		double E=4.0,
				k0=0.4,
				k1=0.02,
				k2=0.4;
		
		int regiao = 3;
		Integer[][] imgLocal; 
		
		ArquivoPGM imgAux;
				
		mediaGlobal = getMediaGlobal();
		System.out.println("Media Global: " + mediaGlobal);
		
		desvioPadraoGlobal = Math.sqrt(getVariancia(mediaGlobal));
		
		System.out.println("Desvio Padrao Global: " + desvioPadraoGlobal);
		Integer[][] novaMatriz = new Integer[altura][largura];
		for(int row=0;row<altura;row++){
			for(int col=0;col<largura;col++){
				novaMatriz[row][col]=0;
			}
		}
		
		for(int row=0;row<altura;row++){
			for(int col=0;col<largura;col++){
				
				imgLocal = new Integer[regiao][regiao];
				
				for(int i=row-1,k=0;i<=row+1;i++,k++){
					for(int j=col-1,l=0;j<=col+1;j++,l++){
						if(i>=0 && i<altura && j>=0 && j<largura){
							imgLocal[k][l] = Matriz[i][j];
						}
						else{
							imgLocal[k][l]=0;
						}
					}
				}
				imgAux = new ArquivoPGM(imgLocal);
				desvioPadraoLocal = Math.sqrt(imgAux.getVariancia(imgAux.getMediaGlobal()));
				mediaLocal = mascara9(row,col); 
				
				if(mediaLocal <= k0*mediaGlobal && (k1*desvioPadraoGlobal <= desvioPadraoLocal && desvioPadraoLocal <= k2*desvioPadraoGlobal)){					
					novaMatriz[row][col] = (int)(E*(double)Matriz[row][col]);					
				}							
			}
		}
		for(int row=0;row<altura;row++){
			for(int col=0;col<largura;col++){
				if(novaMatriz[row][col]>0){
					Matriz[row][col] = novaMatriz[row][col];
				}
			}
		}			
	}

