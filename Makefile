
arpd:
	GOPATH=$(CURDIR) GOBIN=$(CURDIR)/bin go install src/main/arpd.go

harpd:
	GOPATH=$(CURDIR) GOBIN=$(CURDIR)/bin go install src/main/harpd.go


