GOENV=GOPATH=$(CURDIR) GOBIN=$(CURDIR)/bin GOROOT=/opt/go
arpd:
	$(GOENV) go install src/main/arpd.go

harpd:
	$(GOENV) go install src/main/harpd.go


