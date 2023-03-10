USE [LUN-GIP]
GO
/****** Object:  Table [dbo].[Limits]    Script Date: 8/19/2022 12:38:30 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
Drop TABLE [dbo].[Limits]
GO
CREATE TABLE [dbo].[Limits](
	[ID] [uniqueidentifier] NOT NULL,
	[TimeStamp] [datetime] NOT NULL,
	[ErrorCodes] [varchar](50) NOT NULL,
	[LowLimit] [numeric](18, 6) NULL,
	[HighLimit] [numeric](18, 6) NULL,
	[Comparator] [varchar](10) NULL,
	[Units] [varchar](10) NULL,
	[StringComp] [varchar](50) NULL,
	[Models] [varchar](50) NOT NULL,
	[Description] [varchar](50) NULL,
 CONSTRAINT [PK_Limits] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Partnumber]    Script Date: 8/19/2022 12:38:30 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
Drop TABLE [dbo].[Partnumber]
GO
CREATE TABLE [dbo].[Partnumber](
	[ID] [uniqueidentifier] NOT NULL,
	[PartNumberRegex] [varchar](255) NOT NULL,
	[Model] [varchar](10) NOT NULL,
	[Suffix] [varchar](10) NOT NULL,
 CONSTRAINT [PK_Partnumber] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'a2464b18-a217-4be2-a2b2-0282c8d52d21', CAST(N'2022-02-07T10:37:40.210' AS DateTime), N'50001', CAST(3.000000 AS Numeric(18, 6)), CAST(3.500000 AS Numeric(18, 6)), N'GELT', N'VDC', N'VDC', N'GIP', N'3<VDC<3.5')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'a2464b18-a217-4be2-a2b2-0282c8d52d21', CAST(N'2022-02-07T10:37:40.210' AS DateTime), N'50002', CAST(0.000000 AS Numeric(18, 6)), CAST(3.000000 AS Numeric(18, 6)), N'GELT', N'Amps', N'Current', N'GIP', N'0<Amps<3')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'31b3f1ab-26d9-48fa-bd50-1b5b0bd58541', CAST(N'2021-12-21T14:20:02.430' AS DateTime), N'50003', CAST(12.000000 AS Numeric(18, 6)), CAST(15.000000 AS Numeric(18, 6)), N'GELT', N'V', N'VDC ', N'GIP', N'12<VDC<15')
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'718c40f2-ae22-4b50-8993-23f540912c7b', CAST(N'2022-02-02T10:02:42.180' AS DateTime), N'50004', CAST(200.000000 AS Numeric(18, 6)), CAST(250.000000 AS Numeric(18, 6)), N'GELT', N'V', N'VAC', N'GIP', N'200<VAC<250')
GO
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'fa4f72a4-5e99-4093-956d-40a0ba740e36', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'SubM', N'A3')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'54d08a76-3c8c-49c7-9978-941d39cdbd69', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'GIP', N'A2')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'a382ead5-775e-4515-9346-9721282186ab', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{3}))$', N'Nazgul', N'A1')
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_ID]  DEFAULT (newid()) FOR [ID]
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_TimeStamp]  DEFAULT (getdate()) FOR [TimeStamp]
GO
