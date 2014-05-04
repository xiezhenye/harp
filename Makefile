GOENV=GOPATH=$(CURDIR) GOBIN=$(CURDIR)/bin
arpd:
	$(GOENV) go install src/main/arpd.go

harpd:
	$(GOENV) go install src/main/harpd.go


