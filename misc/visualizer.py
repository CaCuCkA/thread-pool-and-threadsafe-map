import os.path
from typing import List

import matplotlib.pyplot as plt
import numpy as np


class Visualizer:
    def __init__(self, avg_time: List[int], min_time: List[int]):
        self.__avg_time = avg_time
        self.__min_time = min_time
        self.__path = "./plots"
        self.__colors = ["pink", "blue"]
        self.__names = ["basic", "on posix api", "on win api", "with local queue", "with work stealing"]
        self.__create_dir()

    def __is_exist(self) -> bool:
        return os.path.exists(self.__path)

    def __create_dir(self) -> None:
        if not self.__is_exist():
            os.makedirs(self.__path)

    def __generate_path(self, name: str):
        return self.__path + "/" + name

    def __plot(self, is_average: bool):
        fig, ax = plt.subplots()
        width = 0.5
        x = np.arange(len(self.__names))

        ax.bar(x, self.__avg_time if is_average else self.__min_time, width=width,
               color=self.__colors[0] if is_average else self.__colors[-1], edgecolor="black", linewidth=0.5)

        ax.set_xticks(x)
        plt.xticks(rotation=10)
        ax.set_xticklabels(self.__names)

        plt.ylabel("time in milliseconds")

        path = self.__generate_path("avg_time" if is_average else "min_time")
        plt.savefig(path)

    def generate_graphics(self):
        self.__plot(True)
        self.__plot(False)
