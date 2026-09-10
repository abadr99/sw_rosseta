CURRENT_PATH = .
SRCS = $(wildcard $(CURRENT_PATH)/*.test)

run-tests:
	lit -v --filter=$(FILTER) $(CURRENT_PATH)  
  
clean:
	@find $(CURRENT_PATH) -type d -name 'Output' -exec rm -rf {} +