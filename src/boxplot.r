plot = read.csv("plot.csv", header = FALSE)
png(filename="plot.png")
par(mfrow=c(1,2))
boxplot(plot$V1, main="Average recorded data", ylab="Time in seconds")
boxplot(plot$V2, main="Recently entered data", ylab="Time in seconds")
dev.off()
