#!/usr/bin/env Rscript

args <- commandArgs(trailingOnly = TRUE)

g_vals <- read.table(args[1])
g_vals[[1]] <- as.numeric(g_vals[[1]])

png(filename = args[2], width = 1000, height = 1000)

#smoothScatter(g_vals[[1]], y = NULL, ylab = "g_chroma values", xlab = "pixel number")
hist(g_vals[[1]], breaks = "sturges", freq = FALSE)

dev.off()