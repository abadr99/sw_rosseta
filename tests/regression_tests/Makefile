CURRENT_PATH = .

run-tests:
	lit -v --filter=$(FILTER) $(CURRENT_PATH)  
  
clean:
	@find $(CURRENT_PATH) -type d -name 'Output' -exec rm -rf {} +