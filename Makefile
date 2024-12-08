
################################################################################
# COMPILATION
################################################################################
CXX				=	c++
CXX_STRICT		=	-Wall -Wextra -Werror -std=c++20
DB_FLAGS		=	-g
HEADERS			=	-I $(INCLUDES)
CXX_FULL		=	$(CXX) $(CXX_STRICT) $(DB_FLAGS) $(HEADERS)

################################################################################
# MANDATORY
################################################################################
NAME			=	webserv
INCLUDES		=	./include
M_HEADERS		=	$(INCLUDES)/Config.hpp \
					$(INCLUDES)/Utils.hpp
OBJ_DIR			=	./obj
SRC_DIR			=	./src
SRCS			=	Config.cpp \
					Utils.cpp \
					main.cpp # Add more sources here

OBJECTS			=	$(SRCS:%.cpp=$(OBJ_DIR)/%.o)

################################################################################
# RULES
################################################################################
vpath %.cpp $(SRC_DIR) \ $(SRC_DIR)/parser  \ $(SRC_DIR)/utils # Add more paths here

all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "--------------------------------------------"
	@$(CXX_FULL) $(OBJECTS) -o $(NAME)
	@echo "[$(NAME)] $(B)Built target $(NAME)$(RC)"
	@echo "--------------------------------------------"

$(OBJ_DIR)/%.o: %.cpp $(M_HEADERS)
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $< to $@"
	@$(CXX_FULL) -c $< -o $@
	@echo "$(G)Compiled: $< $(RC)"

clean:
	@rm -rf $(NAME).dSYM/ $(OBJ_DIR)/
	@echo "[$(NAME)] Object files cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "[$(NAME)] Everything deleted."

re: fclean all
	@echo "[$(NAME)] Everything rebuilt."


################################################################################
# PHONY
################################################################################
.PHONY: all re clean fclean

################################################################################
# Colors
################################################################################
# Green, Blue
R = \033[0;31m
B = \033[0;34m
# Reset Color
RC = \033[0m