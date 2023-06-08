import os
import re
import subprocess as sp
import sys
from typing import List


class Runner:
    def __init__(self, number_of_runs: int, bound_number: int):
        self.__PATH = "./bin/"
        self.__programs = os.listdir(self.__PATH)
        self.__number_of_runs = number_of_runs
        self.__bound_number = bound_number
        self.__average_times: List[float] = list()
        self.__min_times: List[int] = list()

    @staticmethod
    def clear_output(data: str) -> str:
        regex_pattern = r"\n$"
        clear_data = re.sub(regex_pattern, "", data)
        return clear_data

    @staticmethod
    def parse_result(subprocess_entity: sp) -> int:
        result = subprocess_entity.stdout.decode().split(' ')
        if len(result) > 3:
            result.pop()
        time = Runner.clear_output(result[-1])
        return int(time)

    def __generate_path(self, program: str) -> str:
        return self.__PATH + program

    def __generate_arguments_list(self, program_name: str) -> list:
        arguments_list = list()
        program_path = self.__generate_path(program_name)
        arguments_list.append(program_path)
        arguments_list.append(str(self.__bound_number))
        return arguments_list

    def execute(self):
        for program in self.__programs:
            average_time = 0
            min_time = sys.maxsize
            for _ in range(self.__number_of_runs):
                arguments = self.__generate_arguments_list(program)
                subprocess_entity = sp.run(arguments, stdout=sp.PIPE)
                time = Runner.parse_result(subprocess_entity)
                average_time += time
                min_time = min(min_time, time)
            self.__min_times.append(min_time)
            self.__average_times.append(average_time / self.__number_of_runs)

    def get_results(self):
        return self.__average_times, self.__min_times
