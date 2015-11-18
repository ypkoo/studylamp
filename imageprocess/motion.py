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

TIME_LIMIT = 1.0 # Seconds
SIZE_LIMIT = 0.05 # Percentage to the minimum between width and height

class Motion:
	def __init__(self, width, height):
		self.width = width
		self.height = height

		self.points = [] # list of (x, y, t)
		self.clustered_points = [] # list of (x, y, start_index, end_index)

		self.current_cluster = []
		
		self.size_limit_square = (SIZE_LIMIT * min(width, height)) ** 2

	def registerPoint(self, new_point, time):
		''' ASSERT THERE IS NO EMPTY DATA BEFORE THIS POINT '''

		x, y = new_point
		last_x, last_y, last_t = self.points[-1]
		cluster_x, cluster_y = self.clustered_points[-1]

		if (x-cluster_x)**2 + (y-last_y)**2 > self.size_limit_square:
			self.points.
		pass


	def analyze(self):
		return None