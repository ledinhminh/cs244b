.PHONY: doc push clean all

PORT=122
HOST=weishark.stanford.edu

DOCS = report.tex
PDFS = $(DOCS:%.tex=%.pdf)

all: doc push 

doc: $(DOCS)
	pdflatex $<
	rm *.aux *.log 

push:
	$(foreach pdf,$(PDFS),scp -P $(PORT) $(pdf) $(HOST):/)

clean:
	rm -f $(PDFS)

