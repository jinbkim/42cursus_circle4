SRC		= $(wildcard *.cpp)
OBJ		= $(SRC:.cpp=.o)

NAME	= megaphone
RM		= rm -rf
CC		= clang++
FLAGS	= -Wall -Wextra -Werror



all 	: $(NAME)

clean	:
		$(RM) $(OBJ)

fclean	: clean
		$(RM) $(NAME)

re		: fclean all

$(NAME) :
	$(CC) $(FLAGS) -c $(SRC)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)