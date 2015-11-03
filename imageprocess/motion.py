#!/usr/bin/env python
#
# motion.py
# 
# Get various informations from given points.
#
# @author S.H.Lee
#
# @version 1.0
# @since 2015-11-02
# First implementation

class Motion:
	def __init__(self, width, height):
		self.width = width
		self.height = height

		self.points = [] # list of (x, y, t)
		self.start_point = None

	def registerPoint(self, new_point, time):
		pass


	def analyze(self):
		return None