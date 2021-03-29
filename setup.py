### #!/usr/bin/env python
# encoding: utf-8
"""
Python bindings of webrtc audio processing
"""

from glob import glob
import platform
import sys
from setuptools import setup, Extension, find_packages


with open('README.md') as f:
    long_description = f.read()

include_dirs = ['src', 'sv56']

ap_sources = []
ap_dir_prefix = 'sv56/'
for i in range(8):
    ap_sources += glob(ap_dir_prefix + '*.c*')
    ap_dir_prefix += '*/'

print(ap_sources)

sources = (
    ap_sources +
    ['src\\pysv.c', 'src\\pysv.i']
)

setup(
    name='pysv',
    version='0.1.0',
    description='Python bindings of sv56demo in ITU-T G.191)',
    # long_description=long_description,
    # long_description_content_type='text/markdown',
    author='James Park',
    author_email='youngjamespark@gmail.com',
    # url='https://github.com/youngjamespark/python_pysv',
    # download_url='https://github.com/youngjamespark/python_pysv',
    packages=['pysv'],
    ext_modules=[
        Extension(
            name='pysv._pysv',
            sources=sources,
            include_dirs=include_dirs,
        )
    ],
    classifiers=[
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: C'
    ],
    license='BSD',
    keywords=['sv56', 'G.191', 'P.56', 'speech voltmeter', 'normalization'],
    package_dir={
        'pysv': 'src'
    }
)
