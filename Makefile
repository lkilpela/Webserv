NAME = webserv

CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -Iinclude -std=c++17
DEBUG_CXXFLAGS = -g

BUILD_DIR := build

VPATH :=	src \
			src/config \
#src/http \
#src/server

SRCS =	main.cpp \
		Config.cpp \
#Request.cpp \
#Response.cpp \
#Server.cpp

OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=.debug.o)
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) Program Made"

$(BUILD_DIR):
	@mkdir -p $@
	@echo "Compiling ..."

debug: $(DEBUG_OBJS)
	$(CXX) $(DEBUG_OBJS) $(DEBUG_CXXFLAGS) $(CXXFLAGS) -o $(NAME)_debug
	@echo "$(NAME) Debug Program Made"

$(BUILD_DIR)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%.debug.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG_CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEBUG_OBJS)
	@echo "Cleaned object files"

fclean: clean
	$(RM) $(NAME) $(NAME)_debug
	@echo "Fully Cleaned"

re: fclean all

.PHONY: all clean fclean re debug
