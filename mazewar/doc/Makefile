.PHONY: protocol ans clean all

all: protocol ans


protocol: protocol.tex 
	pdflatex $<
	pdflatex $<
	rm *.aux *.log *.toc

ans: answers.tex
	pdflatex $<
	pdflatex $<
	rm *.aux *.log 

clean:
	rm -f protocol.pdf answers.pdf

