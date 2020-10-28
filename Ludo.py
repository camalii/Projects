from random import randint


def dice(prob_six=1 / 6):
    random_number = randint(1, 9223372036854775807)
    max_number = 9223372036854775807
    boundary_percentage = max_number*(1-prob_six)
    if random_number >= boundary_percentage:
        return 6
    my_range = boundary_percentage/5
    roll = int(random_number/my_range)+1
    return roll


def game(size, prob_six=1 / 6, output=True):
    if size < 2:
        if output:
            print("Error: plan too small!")
        return None

    round_count = 0
    roll = 0
    rolled_numbers = []
    final_position = 0

    while roll != 6:
        for i in range(0, 3):
            roll = dice(prob_six)
            if roll == 6:
                rolled_numbers.append(roll)
                final_position = 1
                break
            rolled_numbers.append(roll)
        round_count += 1
        if output:
            print('0 -> ' + str(final_position) + ' in round ' + str(round_count) + " " + (
                str(rolled_numbers).replace('[', '(').replace(']', ')')))
        rolled_numbers.clear()

    final_position = 1
    while final_position <= size:
        starting_position = final_position
        roll = dice(prob_six)
        rolled_numbers.append(roll)
        final_position += roll

        if roll == 6:
            additional_roll = dice(prob_six)
            rolled_numbers.append(additional_roll)
            final_position += additional_roll
            if additional_roll == 6:
                second_additional_roll = dice(prob_six)
                rolled_numbers.append(second_additional_roll)
                final_position += second_additional_roll

        if sum(rolled_numbers) + starting_position > size:
            final_position = starting_position

        round_count += 1
        if output:
            print(str(starting_position) + " -> " + str(final_position) + " in round " + str(round_count) + " " + (
                str(rolled_numbers).replace('[', '(').replace(']', ')')))
        rolled_numbers.clear()

        if final_position == size:
            if output:
                print("Game finished in round " + str(round_count) + ".")
            return round_count


def average_game(size, games_count, prob_six=1 / 6):
    average_game_length = 0
    for i in range(0, games_count):
        round_count = game(size, prob_six, False)
        average_game_length += round_count
    return average_game_length / games_count


if __name__ == '__main__':
    print(game(44))
    print(average_game(15, 5))
