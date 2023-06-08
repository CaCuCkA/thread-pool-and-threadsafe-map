import logging
import sys

from misc.runner import Runner
from misc.visualizer import Visualizer


def input_data():
    if len(sys.argv) < 2:
        logging.error("Not enough arguments")
        exit(1)
    number_of_runs = int(sys.argv[1])
    bound_number = int(sys.argv[2])

    return number_of_runs, bound_number


def main():
    number_of_runs, bound_number = input_data()
    runner = Runner(number_of_runs=number_of_runs, bound_number=bound_number)
    runner.execute()
    avg_times, min_times = runner.get_results()

    visualizer = Visualizer(avg_time=avg_times, min_time=min_times)
    visualizer.generate_graphics()


if __name__ == '__main__':
    main()
