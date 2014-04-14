from setuptools import setup

setup(
    name="denali",
    version="1.0",
    description="Utilities for use with the denali visualization suite.",

    author="Justin Eldridge",
    author_email="eldridge@cse.ohio-state.edu",

    packages=["denali"],

    extras_require = {
        "numpy": ["numpy"],
        "sklearn": ["scikit-learn"],
        "networkx": ["networkx"]}
)
